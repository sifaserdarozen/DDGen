pipeline {
    agent any
    stages {
        stage('Preperation') {
            failFast false
            parallel {
                stage('cppcheck') {
                    steps {
                        sh 'echo "cppcheck ..."'
                    }
                }
                stage('check docker') {
                    steps {
                        sh 'echo "docker check ..."'
                    }
                }
            }
        }

        stage('Build') {
            steps {
                sh 'echo "docker build ..."'
                sh 'docker build -t ddgen:test .'
            }
        }
    }
}