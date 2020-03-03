﻿/*
* Copyright 2010-2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License").
* You may not use this file except in compliance with the License.
* A copy of the License is located at
*
*  http://aws.amazon.com/apache2.0
*
* or in the "license" file accompanying this file. This file is distributed
* on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
* express or implied. See the License for the specific language governing
* permissions and limitations under the License.
*/

#include <aws/dynamodb/model/RestoreTableFromBackupRequest.h>
#include <aws/core/utils/json/JsonSerializer.h>

#include <utility>

using namespace Aws::DynamoDB::Model;
using namespace Aws::Utils::Json;
using namespace Aws::Utils;

RestoreTableFromBackupRequest::RestoreTableFromBackupRequest() : 
    m_targetTableNameHasBeenSet(false),
    m_backupArnHasBeenSet(false),
    m_billingModeOverride(BillingMode::NOT_SET),
    m_billingModeOverrideHasBeenSet(false),
    m_globalSecondaryIndexOverrideHasBeenSet(false),
    m_localSecondaryIndexOverrideHasBeenSet(false),
    m_provisionedThroughputOverrideHasBeenSet(false)
{
}

Aws::String RestoreTableFromBackupRequest::SerializePayload() const
{
  JsonValue payload;

  if(m_targetTableNameHasBeenSet)
  {
   payload.WithString("TargetTableName", m_targetTableName);

  }

  if(m_backupArnHasBeenSet)
  {
   payload.WithString("BackupArn", m_backupArn);

  }

  if(m_billingModeOverrideHasBeenSet)
  {
   payload.WithString("BillingModeOverride", BillingModeMapper::GetNameForBillingMode(m_billingModeOverride));
  }

  if(m_globalSecondaryIndexOverrideHasBeenSet)
  {
   Array<JsonValue> globalSecondaryIndexOverrideJsonList(m_globalSecondaryIndexOverride.size());
   for(unsigned globalSecondaryIndexOverrideIndex = 0; globalSecondaryIndexOverrideIndex < globalSecondaryIndexOverrideJsonList.GetLength(); ++globalSecondaryIndexOverrideIndex)
   {
     globalSecondaryIndexOverrideJsonList[globalSecondaryIndexOverrideIndex].AsObject(m_globalSecondaryIndexOverride[globalSecondaryIndexOverrideIndex].Jsonize());
   }
   payload.WithArray("GlobalSecondaryIndexOverride", std::move(globalSecondaryIndexOverrideJsonList));

  }

  if(m_localSecondaryIndexOverrideHasBeenSet)
  {
   Array<JsonValue> localSecondaryIndexOverrideJsonList(m_localSecondaryIndexOverride.size());
   for(unsigned localSecondaryIndexOverrideIndex = 0; localSecondaryIndexOverrideIndex < localSecondaryIndexOverrideJsonList.GetLength(); ++localSecondaryIndexOverrideIndex)
   {
     localSecondaryIndexOverrideJsonList[localSecondaryIndexOverrideIndex].AsObject(m_localSecondaryIndexOverride[localSecondaryIndexOverrideIndex].Jsonize());
   }
   payload.WithArray("LocalSecondaryIndexOverride", std::move(localSecondaryIndexOverrideJsonList));

  }

  if(m_provisionedThroughputOverrideHasBeenSet)
  {
   payload.WithObject("ProvisionedThroughputOverride", m_provisionedThroughputOverride.Jsonize());

  }

  return payload.View().WriteReadable();
}

Aws::Http::HeaderValueCollection RestoreTableFromBackupRequest::GetRequestSpecificHeaders() const
{
  Aws::Http::HeaderValueCollection headers;
  headers.insert(Aws::Http::HeaderValuePair("X-Amz-Target", "DynamoDB_20120810.RestoreTableFromBackup"));
  return headers;

}




