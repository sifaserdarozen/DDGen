def String image = ''
def String appImage = ''
def String hostname = ''

pipeline {
    agent any
    stages {
        stage('Linting ...') {
            failFast false
            parallel {
                stage('cppcheck') {
                    steps {
                        sh 'echo "cppcheck ..."'
                        sh 'cppcheck -v --error-exitcode=1 --enable=style,performance,portability --inconclusive -I include --inline-suppr src' 
                    }
                }
                stage('hadolint') {
                    steps {
                        sh 'echo "hadolint ..."'
                        sh 'hadolint --ignore DL3018 docker/Dockerfile'
                    }
                }
                stage('clang-format') {
                    steps {
                        sh 'echo "clang-format ..."'
                        sh 'clang-format -i src/**.cpp include/**.h'
                        sh 'git diff --exit-code'
                    }
                }
            }
        }

        stage('Build') {
            steps {
                script {
                    sh 'echo "building image ..."'
                    sh 'docker build -t ddgen -f docker/Dockerfile .'

                    image = sh(script:'echo "sifaserdarozen/ddgen:$(date +%Y.%m.%d-%H.%M)-$(git rev-parse --short HEAD)"', returnStdout: true).trim().toLowerCase()
                    echo "image: ${image}"
                    sh "docker tag ddgen:latest ${image}"

                    sh 'docker image list'
                }
            }
        }

        stage('Test') {
            steps {
                sh 'echo "running unit tests ..."'
                script {
                    sh "docker run --entrypoint=./ddgen_utests ${image}"
                }
            }
        }

        stage('Publish') {
            steps {
                script {
                    sh 'echo "publishing image ..."'
                    withCredentials([usernamePassword(credentialsId: 'dockerHubCredentials', usernameVariable: 'dockerUsername', passwordVariable: 'dockerPassword')]) {
                        sh "docker login -u ${dockerUsername} -p ${dockerPassword}"
                        sh "docker push ${image}"
                    }
                }
            }
        }

        stage('Deploy') {
            steps {
                script {
                    sh 'echo "generating deployment file with helm"'
                    sh "helm template --set image=${image} kubernetes > kubernetes/deployment.yaml"
                    sh "cat kubernetes/deployment.yaml"
                    echo "deploying with image: ${image}"
                    withAWS(region:'us-east-2', credentials:'eksCredentials') {
                        sh "kubectl config use-context arn:aws:eks:us-east-2:313595130251:cluster/ddgen"
                        sh "kubectl apply -f kubernetes/deployment.yaml"
                        sh "kubectl rollout status deployment/ddgen"
                        sh "kubectl get service --selector=app=ddgen -o jsonpath={.items[0].status.loadBalancer.ingress[0].hostname}"
                    }
                }
            }
        }

        stage('Verification') {
            steps {
                script {
                    sh 'echo "verification..."'
                    withAWS(region:'us-east-2', credentials:'eksCredentials') {
                        hostname = sh(script:'kubectl get service --selector=app=ddgen -o jsonpath={.items[0].status.loadBalancer.ingress[0].hostname}', returnStdout: true)
                        appImage = sh(script:"curl ${hostname}:8080/image", returnStdout: true)
                        if (image.equals( appImage )) {
                            echo "verification is ok, image querry ${appImage} is equal to docker image"
                        } else {
                            echo "verification failed, image querry ${appImage} is not equal to docker image ${image}"
                            sh "kubectl rollout undo deployment/ddgen"
                            currentBuild.result = 'FAILURE'
                        }
                    }
                }
            }
        }

        stage('Cleanup') {
            steps {
                sh 'echo "cleaning ..."'
                sh 'docker system prune -a -f'
            }
        }
    }
}