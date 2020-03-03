NEVER PUT PRODUCTION CREDENTIALS TO REPOSITORY
ALL FILES EXCEPT THIS WILL BE IGNORED BY.GITIGNORE

If an aws credentials as environment variables file is put here, 
it will be copied to container.

---------------- save as env.list------------------
export AWS_DEFAULT_REGION=us-east-2
export AWS_ACCESS_KEY_ID=SomeKeyId
export AWS_SECRET_ACCESS_KEY=SomeSecretAccessKey
---------------------------------------------------

and use as
docker run --env-file docker/aws/env.list ddgen /ddgen/ddgen --nc 2 --dc 10 --ds 30 --mirror --useDb --useS3
