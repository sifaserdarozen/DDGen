aws cloudformation create-stack \
--stack-name ddgen \
--template-body file://./cloudformation/eks-template.yaml \
--parameters file://./cloudformation/eks-parameters.json \
--region us-east-2 \
--capabilities CAPABILITY_IAM CAPABILITY_NAMED_IAM