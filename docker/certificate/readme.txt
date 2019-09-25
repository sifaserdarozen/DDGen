NEVER PUT PRODUCTION CERTIFICATES TO REPOSITORY
ALL FILES EXCEPT THIS WILL BE IGNORED BY.GITIGNORE

If a server certificate is put here, it will be copied to container.
When secure connection is requested by --secure flag, and there is no
ddgen-cert.pem copied from here, it will be generated inside the container.
