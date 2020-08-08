def String image = ''
def dockerImage = ''

pipeline {
    environment {
        dockerCredentials = 'dockerHubCredentials'
    }
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
                    // TODO REPLACE DOCKER PULL WITH BUILD STEPS
                    sh 'docker pull sifaserdarozen/ddgen:latest'
                    // image = sh(script:'echo "sifaserdarozen/ddgen/$(git rev-parse --abbrev-ref HEAD):$(date +%Y.%m.%d-%H.%M)-$(git rev-parse --short HEAD)"', returnStdout: true).trim().toLowerCase()
                    image = sh(script:'echo "sifaserdarozen/ddgen:$(date +%Y.%m.%d-%H.%M)-$(git rev-parse --short HEAD)"', returnStdout: true).trim().toLowerCase()
                    echo "image: ${image}"
                    sh "docker tag sifaserdarozen/ddgen:latest ${image}"
                    sh 'docker image list'

                    // dockerImage = docker.build("${image}", "./docker/")
                }
            }
        }

        stage('Test') {
            steps {
                sh 'echo "running unit tests ..."'
                script {
                    sh "docker run --entrypoint=./ddgen_utests ${image}"
                }

                //script {
                //    docker.image("${image}").inside("-i --entrypoint=/bin/sh") {
                //        sh "pwd"
                //        sh "ls -al"
                //        sh "./ddgen_utests"
                //    }
                //}
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
                    // docker.withRegistry( '', dockerCredentials )
                    // dockerImage.push()
                }
            }
        }

        stage('Deploy') {
            steps {
                sh 'echo "deploying ..."'
                echo "deploying with image: ${image}"
                // TODO DEPLOY STEPS
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