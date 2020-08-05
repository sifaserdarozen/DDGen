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
                sh 'echo "building ..."'
                // TODO BUILD STEPS
            }
        }
        
        stage('Deploy') {
            steps {
                sh 'echo "deploying ..."'
                // TODO DEPLOY STEPS
            }
        }
    }
}