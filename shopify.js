/**
 * @file
 * Contains javascript specific to Shopify module functionality.
 */
(function ($) {

  Drupal.shopify = {
    ctx: {},
    settings: {}
  };

  /**
   * Display an "Added to cart" message by sending a POST request to the backend.
   */
  Drupal.shopify.display_add_to_cart_message = function () {
    var $forms = $('form.shopify-add-to-cart-form');
    $forms.unbind('submit').submit(function (e) {
      var $form = $(this);
      e.preventDefault();
      $.post(Drupal.settings.basePath + '?q=shopify/added-to-cart', {
        product_id: $form.data('product-id'),
        variant_id: $form.data('variant-id'),
        quantity: $form.find('input[name="quantity"]').val()
      }, function (data) {
        $form.get(0).submit();
      });
    });
  };

  /**
   * Displays the cart quantity.
   */
  Drupal.shopify.updateCartQuantity = function () {
    var url = $('#shopify-cart-link').attr('href');
    $.ajax({
      type: 'GET',
      url: url + '.json',
      dataType: 'jsonp',
      success: function (data) {
        var total = 0;
        for (i = 0; i < data.items.length; i++) {
          total += data.items[i].quantity;
        }
        $('#shopify-cart-total').text(total);
      }
    });
  };

  Drupal.behaviors.shopify = {
    attach: function (context, settings) {
      Drupal.shopify.ctx = context;
      Drupal.shopify.settings = settings;
      Drupal.shopify.display_add_to_cart_message();
      Drupal.shopify.updateCartQuantity();
    }
  }

}(jQuery));