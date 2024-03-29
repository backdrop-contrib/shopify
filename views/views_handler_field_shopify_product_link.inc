<?php
/**
 * @file
 * Field handler for product links.
 */

class views_handler_field_shopify_product_link extends views_handler_field_entity {

  public function option_definition() {
    $options = parent::option_definition();
    $options['text'] = array(
      'default' => '',
      'translatable' => TRUE,
    );
    return $options;
  }

  public function options_form(&$form, &$form_state) {
    $form['text'] = array(
      '#type' => 'textfield',
      '#title' => t('Text to display'),
      '#default_value' => $this->options['text'],
    );
    parent::options_form($form, $form_state);

    $form['alter']['path'] = array('#access' => FALSE);
    $form['alter']['external'] = array('#access' => FALSE);
  }

  /**
   * Render.
   */
  public function render($values) {
    if ($entity = $this->get_value($values)) {
      return $this->render_link($entity, $values);
    }
  }

  function render_link($entity, $values) {
    $this->options['alter']['make_link'] = TRUE;
    $uri = entity_uri($entity->entityType(), $entity);
    $this->options['alter']['path'] = $uri['path'];
    $text = !empty($this->options['text']) ? $this->options['text'] : t('view');

    return $text;
  }
}
