<?php
/**
 * @file
 * Contains functions for working with the Shopify API.
 */

use Osiset\BasicShopifyAPI\BasicShopifyAPI;
use Osiset\BasicShopifyAPI\Options;
use Osiset\BasicShopifyAPI\Session;

// Sleep timeout in ms.
define('SHOPIFY_API_CALL_TIMEOUT', 500);

// Required libraries
include_once(backdrop_get_path('module', 'shopify') . '/library/BasicShopifyAPI/vendor/autoload.php');

/**
 * Create a Shopify API client class.
 *
 * @return
 *   A BasicShopifyAPI class if all connection settings are present, otherwise
 *   NULL.
 */
function shopify_api_get_client() {

  // Mark what is required.
  $required = array('domain', 'token', 'secret', 'key');

  // Iterate the required items and fetch the data.
  foreach ($required as $key) {
    // If we cannot get a value, then we cannot get a client.
    $setting = config_get('shopify.settings', "api_" . $key);
    if (!(${$key} = $setting)) {
      return NULL;
    }
  }

  // Create options for the API
  $options = new Options();
  $options->setVersion('2022-01');
  $options->setApiKey($key);
  $options->setApiPassword($secret);

  // Create the client and session
  $client = new BasicShopifyAPI($options);
  $client->setSession(new Session($domain, $token));

  return $client;

}

/**
 * Perform an API call to Shopify.
 *
 * @param string $method
 *   HTTP method: GET, POST, PUT, DELETE.
 * @param string $path
 *   API path to the resource.
 * @param array $params
 *   Parameters to pass to the API call.
 *
 * @return array|int
 *   Returns the API response data. May be an array or count.
 *
 * @throws \ShopifyApiException
 */
function shopify_api_call($method, $path, array $params = array()) {
  if ($timeout = config_get('shopify.settings', 'api_next_timeout')) {
    // Sleep for a while to let the bucket empty out a bit.
    usleep($timeout * 1000);
    config_set('shopify.settings', 'api_next_timeout', 0);
  }
  $client = shopify_api_get_client();
  if (!$client instanceof BasicShopifyAPI) {
    throw new Exception('Unable to create BasicShopifyAPI. Check your Shopify API credentials at /admin/shopify/api');
  }
  $result = $client->rest($method, '/admin/' . $path . '.json', $params);
  if (shopify_api_call_limit_reached($client)) {
    config_set('shopify.settings', 'api_next_timeout', shopify_api_get_next_timeout());
  }
  // dpm(__FUNCTION__);
  // dpm($method);
  // dpm('/admin/' . $path . '.json');
  // dpm($params);
  if (!empty($result['body']->container[$path])) {
    return $result['body']->container[$path];
  }
  elseif (!empty($result['body']->container['count'])) {
    return $result['body']->container['count'];
  }
  else {
    return 0;
  }
}

/**
 * Gets a somewhat random timeout, between 500-2000ms.
 * This is the best chance at allowing the "leaky bucket" to refill.
 *
 * Timeout can be disabled by setting "shopify_api_call_timeout" to 0.
 * Timeout randomizer can be disabled by setting "shopify_api_call_timeout_multiplier" to 1.
 *
 * @return int
 *   Number of ms to delay.
 */
function shopify_api_get_next_timeout() {
  $config = config('shopify.settings');
  return $config->get('api_call_timeout') * rand(1, $config->get('api_call_timeout_multiplier'));
}

/**
 * Determines if the API call limits have been reached and should be throttled.
 *
 * @param \BasicShopifyAPI $client
 *   Shopify API client.
 *
 * @return bool
 *   Returns TRUE if the following call should be throttled.
 */
function shopify_api_call_limit_reached(BasicShopifyAPI $client) {
  // Temporarily disable as new library includes basic rate limiting OOTB.
  return 0;

  try {
    if ($client->callsMade() >= ($client->callLimit() - 2)) {
      return TRUE;
    }
  }
  catch (Exception $e) {
    // Cannot get the callsMade(), can safely assume limit is not reached.
    return FALSE;
  }
  return FALSE;
}

/**
 * Gets the Shopify shop info from the API or the variable.
 *
 * @param bool $refresh
 *   Whether to refresh the info from the API.
 *
 * @return object
 *   Return object of store information.
 *
 * @throws \ShopifyApiException
 */
function shopify_api_get_shop_info($refresh = FALSE) {
  $info = (array) shopify_store_info();
  if ($refresh == TRUE || empty($info)) {
    $info = shopify_api_call('GET', 'shop');
    config_set('shopify.settings', 'store_info', $info);
  }
  return (object) $info;
}

/**
 * Gets the admin path for the Shopify store.
 */
function shopify_api_get_shop_admin_path() {
  return 'https://' . shopify_api_get_shop_info()->domain . '/admin';
}

/**
 * Gets the Shopify store product path.
 *
 * @param int $product_id
 *   Product ID.
 * @param int $variant_id
 *   Variant ID.
 *
 * @return string
 *   Product path.
 */
function shopify_api_get_product_path($product_id, $variant_id = 0) {
  $variant_path = '';
  if ($variant_id != 0) {
    $variant_path = '/variants/' . $variant_id;
  }
  return shopify_api_get_shop_admin_path() . '/products/' . $product_id . $variant_path;
}

/**
 * Get Shopify products from the API.
 *
 * @param array $params
 *   Parameters to pass to the API.
 *
 * @return array
 *   Returns the API response data.
 *
 * @throws \ShopifyApiException
 */
function shopify_api_get_products(array $params = array()) {
  return shopify_api_call('GET', 'products', $params);
}

/**
 * Get Shopify collections from the API.
 *
 * @param array $params
 *   Parameters to pass to the API.
 * @param string $type
 *   Type of collections to get, options are:
 *   - both: Get smart and custom collections.
 *   - smart: Get smart collections only.
 *   - custom: Get custom collections only.
 *
 * @return array
 *   Returns the API response data.
 *
 * @throws \ShopifyApiException
 */
function shopify_api_get_collections(array $params = array(), $type = 'both') {
  $smart_collections = $custom_collections = array();
  if ($type == 'both' || $type == 'smart') {
    $smart_collections = shopify_api_call('GET', 'smart_collections', $params);
  }
  if ($type == 'both' || $type == 'custom') {
    $custom_collections = shopify_api_call('GET', 'custom_collections', $params);
  }
  $return_array = array();
  if (!empty($smart_collections)) {
    $return_array = $smart_collections;
  }
  if (!empty($custom_collections)) {
    $return_array = array_merge($return_array, $custom_collections);
  }
  return $return_array;
}

/**
 * Get Shopify collects from the API.
 *
 * @param array $params
 *   Arguments to pass the API.
 *
 * @return array
 *   Returns the API response data.
 *
 * @throws \ShopifyApiException
 */
function shopify_api_get_collects(array $params = array()) {
  return shopify_api_call('GET', 'collects', $params);
}

/**
 * Get the Shopify product count from the API.
 *
 * @param array $params
 *   Parameters to pass to the API.
 *
 * @return int
 *   Returns the product count.
 *
 * @throws \ShopifyApiException
 */
function shopify_api_get_product_count(array $params = array()) {
  return shopify_api_call('GET', 'products/count', $params);
}

/**
 * Get the Shopify variants count for a specific product from the API.
 *
 * @param int $product_id
 *   Product ID.
 * @param array $params
 *   Parameters to pass to the API.
 *
 * @return int
 *   Returns the number of variants for this product.
 *
 * @throws \ShopifyApiException
 */
function shopify_api_get_product_variants_count($product_id, array $params = array()) {
  return shopify_api_call('GET', "products/$product_id/variants/count", $params);
}

/**
 * Get Shopify collections count from the API.
 *
 * @param array $params
 *   Parameters to pass to the API.
 * @param string $type
 *   Type of collections to count, options are:
 *   - both: Get smart and custom collections count.
 *   - smart: Get smart collections count only.
 *   - custom: Get custom collections count only.
 *
 * @return int
 *   Returns the collection count.
 *
 * @throws \ShopifyApiException
 */
function shopify_api_get_collection_count(array $params = array(), $type = 'both') {
  $smart_count = $custom_count = 0;
  if ($type == 'both' || $type == 'smart') {
    $smart_count = shopify_api_call('GET', 'smart_collections/count', $params);
  }
  if ($type == 'both' || $type == 'custom') {
    $custom_count = shopify_api_call('GET', 'custom_collections/count', $params);
  }
  return $custom_count + $smart_count;
}

/**
 * Get the Shopify product information.
 *
 * @param int $product_id
 *   Shopify Product ID.
 * @param array $params
 *   Parameters to pass to the API.
 *
 * @return array
 *   Returns the API response data.
 *
 * @throws \ShopifyApiException
 */
function shopify_api_get_product($product_id, array $params = array()) {
  return shopify_api_call('GET', "products/$product_id", $params);
}

/**
 * Get the Shopify product variants.
 *
 * @param int $product_id
 *   Shopify Product ID.
 * @param array $params
 *   Parameters to pass to the API.
 *
 * @return array
 *   Returns the API response data.
 *
 * @throws \ShopifyApiException
 */
function shopify_api_get_product_variants($product_id, array $params = array()) {
  return shopify_api_call('GET', "products/$product_id/variants", $params);
}

/**
 * Get the Shopify variant information.
 *
 * @param int $variant_id
 *   Shopify Variant ID.
 * @param array $params
 *   Parameters to pass to the API.
 *
 * @return array
 *   Returns the API response data.
 *
 * @throws \ShopifyApiException
 */
function shopify_api_get_variant($variant_id, array $params = array()) {
  return shopify_api_call('GET', "variants/$variant_id", $params);
}

/**
 * Get the Shopify product images.
 *
 * @param int $product_id
 *   Shopify Product ID.
 *
 * @return array
 *   Returns the API response data.
 *
 * @throws \ShopifyApiException
 */
function shopify_api_get_product_images($product_id) {
  return shopify_api_call('GET', "products/$product_id/images");
}

/**
 * Get Shopify webhooks.
 *
 * @param array $params
 *   Parameters to pass to the API.
 *
 * @return array
 *   Returns the API response data.
 *
 * @throws \ShopifyApiException
 */
function shopify_api_get_webhooks(array $params = array()) {
  return shopify_api_call('GET', 'webhooks', $params);
}

/**
 * Get the Shopify webhooks count.
 *
 * @param array $params
 *   Parameters to pass to the API.
 *
 * @return int
 *   Returns the webhooks count.
 *
 * @throws \ShopifyApiException
 */
function shopify_api_get_webhook_count(array $params = array()) {
  return shopify_api_call('GET', 'webhooks/count', $params);
}

/**
 * Creates a new Shopify webhook.
 *
 * @param array $post
 *   Data to POST to the API.
 *
 * @return array
 *   Returns the API response data.
 *
 * @throws \ShopifyApiException
 */
function shopify_api_create_webhook($post) {
  return shopify_api_call('POST', 'webhooks', $post);
}

/**
 * Deletes a new Shopify webhook.
 *
 * @param int $id
 *   Webhook ID to delete.
 *
 * @return array
 *   Returns the API response data.
 *
 * @throws \ShopifyApiException
 */
function shopify_api_delete_webhook($id) {
  return shopify_api_call('DELETE', "webhooks/$id");
}
