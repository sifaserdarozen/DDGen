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

#include <aws/dynamodb/model/TransactGetItemsRequest.h>
#include <aws/core/utils/json/JsonSerializer.h>

#include <utility>

using namespace Aws::DynamoDB::Model;
using namespace Aws::Utils::Json;
using namespace Aws::Utils;

TransactGetItemsRequest::TransactGetItemsRequest() : 
    m_transactItemsHasBeenSet(false),
    m_returnConsumedCapacity(ReturnConsumedCapacity::NOT_SET),
    m_returnConsumedCapacityHasBeenSet(false)
{
}

Aws::String TransactGetItemsRequest::SerializePayload() const
{
  JsonValue payload;

  if(m_transactItemsHasBeenSet)
  {
   Array<JsonValue> transactItemsJsonList(m_transactItems.size());
   for(unsigned transactItemsIndex = 0; transactItemsIndex < transactItemsJsonList.GetLength(); ++transactItemsIndex)
   {
     transactItemsJsonList[transactItemsIndex].AsObject(m_transactItems[transactItemsIndex].Jsonize());
   }
   payload.WithArray("TransactItems", std::move(transactItemsJsonList));

  }

  if(m_returnConsumedCapacityHasBeenSet)
  {
   payload.WithString("ReturnConsumedCapacity", ReturnConsumedCapacityMapper::GetNameForReturnConsumedCapacity(m_returnConsumedCapacity));
  }

  return payload.View().WriteReadable();
}

Aws::Http::HeaderValueCollection TransactGetItemsRequest::GetRequestSpecificHeaders() const
{
  Aws::Http::HeaderValueCollection headers;
  headers.insert(Aws::Http::HeaderValuePair("X-Amz-Target", "DynamoDB_20120810.TransactGetItems"));
  return headers;

}




