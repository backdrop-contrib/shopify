<?php

/**
 * Admin settings form.
 */
function shopify_api_admin_settings($form, &$form_state) {
  $config = config('shopify.settings');
  module_load_install('shopify');

  // Requirements
  $form['#config'] = 'shopify.settings';
  $form['requirements'] = array(
    '#type' => 'fieldset',
    '#title' => t('Requirements'),
  );
  $form['requirements']['report'] = array(
    '#markup' => theme('status_report', array('requirements' => shopify_requirements('runtime'))),
  );

  // Connection
  $form['connection'] = array(
    '#type' => 'fieldset',
    '#title' => t('Connection'),
  );
  $form['connection']['api_domain'] = array(
    '#type' => 'textfield',
    '#title' => t('Domain'),
    '#required' => TRUE,
    '#default_value' => $config->get('api_domain'),
    '#description' => t('Do not include http:// or https://. This is the domain ending with .myshopify.com.'),
  );
  $form['connection']['api_token'] = array(
    '#type' => 'textfield',
    '#title' => t('Token'),
    '#required' => TRUE,
    '#default_value' => $config->get('api_token'),
    '#description' => t('This may be labeled as <em>Password</em> in the Shopify private apps console.'),
  );
  $form['connection']['api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('API key'),
    '#required' => TRUE,
    '#default_value' => $config->get('api_key'),
  );
  $form['connection']['api_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Secret'),
    '#required' => TRUE,
    '#default_value' => $config->get('api_secret'),
  );
  $form['connection']['help'] = array(
    '#type' => 'details',
    '#summary' => t('Help'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['connection']['help']['list'] = array(
    '#theme' => 'item_list',
    '#type' => 'ol',
    '#items' => array(
      t('Log in to your Shopify store in order to access the administration section.'),
      t('Click on "Apps" on the left-side menu.'),
      t('Click "Private Apps" on the top-right of the page.'),
      t('Enter a name for the application. This is private and the name does not matter.'),
      t('Click "Save App".'),
      t('Enter the "Password" as the "Token" value.'),
      t('Enter the "API key" and "Secret" values.'),
      t('Enter your Shopify store URL as the "Domain". It should be in the format of (store-name).myshopify.com.'),
      t('Click "Save configuration".'),
    ),
  );

  return system_settings_form($form);
}
