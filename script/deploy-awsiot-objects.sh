#! /bin/sh
# Workshop: Integrate the AWS Cloud with Responsive Xilinx Machine Learning at the Edge
# Copyright (C) 2018 Amazon.com, Inc. All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#
# deploy-awsiot-objects.sh
#
# Create AWS IoT objects for the lab.
#

prefix=$1

if test -z "$prefix"; then
    echo "You must have a PREFIX defined as first argument to this script."
else
    thing_afr=${prefix}-node-flipnclick-pic32mz
fi

policy_afr=${thing_afr}-policy

dc_afr=$(dirname $0)/../../edge/auth-${thing_afr}

if test -d ${dc_afr}; then
  echo The Amazon FreeRTOS certificate and key for this prefix seem to exist already.
  echo The only reason to do this again is if you change regions.
  echo If you are not changing regions, please proceed with the next lab step.
  exit 0
fi

mkdir -p ${dc_afr}

# AWS provisioned Certificate for Amazon FreeRTOS
echo Creating AWS generated Key and Certificate for [${thing_afr}].
cert_arn_afr=$(aws iot create-keys-and-certificate --output text              \
                   --set-as-active                                            \
                   --certificate-pem-outfile $dc_afr/${thing_afr}.crt.pem     \
                   --public-key-outfile      $dc_afr/${thing_afr}.key.pub.pem \
                   --private-key-outfile     $dc_afr/${thing_afr}.key.prv.pem \
                   --query certificateArn)

echo Creating thing [${thing_afr}].
thing_arn_afr=$(aws iot create-thing --output text         \
                    --thing-name ${thing_afr} \
                    --query thingArn)

# eventual consistency
sleep 5

echo Attaching thing [${thing_afr}] to its certificate.
aws iot attach-thing-principal   \
    --thing-name ${thing_afr}    \
    --principal  ${cert_arn_afr}

cat <<EOF > $dc_afr/policy.json
{
    "Version": "2012-10-17",
    "Statement": [
        {
            "Effect": "Allow",
            "Action": "iot:Connect",
            "Resource": "arn:aws:iot:*:*:client/${iot:Connection.Thing.ThingName}"
        },
        {
            "Effect": "Allow",
            "Action": "iot:Publish",
            "Resource": "arn:aws:iot:*:*:*"
        },
        {
            "Effect": "Allow",
            "Action": "iot:Subscribe",
            "Resource": "arn:aws:iot:*:*:*"
        },
        {
            "Effect": "Allow",
            "Action": "iot:Receive",
            "Resource": "arn:aws:iot:*:*:*"
        }
    ]
}
EOF

echo Creating policy [${policy_afr}].
aws iot create-policy --output text                \
    --policy-name ${policy_afr}                    \
    --policy-document file://${dc_afr}/policy.json \
    --query policyArn

# Potential for eventual consistency here.
sleep 5

echo Attaching policy [${policy_afr}] to its certificate.
aws iot attach-principal-policy \
    --policy-name ${policy_afr} \
    --principal ${cert_arn_afr}


echo Done!
echo Configure aws_clientcredential to use:
echo clientcredentialIOT_THING_NAME : ${thing_afr}
echo clientcredentialMQTT_BROKER_ENDPOINT[] : $(aws iot describe-endpoint --output text --query endpointAddress)
echo clientcredentialWIFI_SSID : self defined
echo clientcredentialWIFI_PASSWORD : self defined
echo ""
echo Configure aws_clientcredential_keys.h using cert and key at ${dc_afr}
echo Use header file tool at amazon-freertos/tools/certificate_configuration/CertificateConfigurator.html
echo Place the generated header file to amazon-freertos/demos/common/include.
