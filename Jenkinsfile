pipeline {
    agent any
    stages {
        stage('linting ...') {
            failFast false
            parallel {
                stage('cppcheck') {
                    steps {
                        sh 'echo "cppcheck ..."'
                        sh 'pwd'
                        sh 'cppcheck --enable=all --inconclusive --xml --xml-version=2 -I include -I include src 2> cppcheck.xml' 
                    }
                }
                stage('hadolint') {
                    steps {
                        sh 'echo "hadolint ..."'
                        sh 'pwd'
                        sh 'hadolint --ignore DL3018 docker/Dockerfile'
                    }
                }
                stage('clang-format') {
                    steps {
                        sh 'pwd'
                        sh 'echo "clang-format ..."'
                    }
                }
            }
        }

        stage('Build') {
            steps {
                sh 'echo "building ..."'
                sh 'docker build -t ddgen:cloudformation -f docker/Dockerfile.'
            }
        }
        
        stage('Deploy') {
            steps {
                sh 'echo "deploying ..."'
            }
        }
    }
}