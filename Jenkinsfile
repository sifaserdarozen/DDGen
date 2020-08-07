def String image = ''

pipeline {
    agent any
    stages {
        stage('linting ...') {
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
                sh 'echo "building image ..."'
                // TODO BUILD STEPS
            }
        }

        stage('Publish') {
            steps {
                script {
                    sh 'echo "publishing image ..."'
                    sh 'docker pull hello-world'
                    image = sh(script:'echo "sifaserdarozen/hello-world/$(git rev-parse --abbrev-ref HEAD):$(date +%Y.%m.%d-%H.%M)-$(git rev-parse --short HEAD)"', returnStdout: true).trim().toLowerCase()
                    echo "image: ${image}"
                    sh "docker tag hello-world ${image}"
                    sh 'docker image list'
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