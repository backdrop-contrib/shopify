
# Shopify

What does this module do?
- This module provides seamless integration of Shopify with your Backdrop site.
- Products, images, tags and collections are synced instantly via webhooks or on demand via batches.
- Complete webhook support for Shopify events. Listen for events in Shopify and trigger your own events in code. (Rules support coming soon.)
- Product tags and collections are represented by customizable taxonomy terms and pages.
- Products, tags, and collections are fieldable. Add your own custom fields to Shopify's. Custom field values are preserved when products are synced.
- Complete Views support for all Shopify product fields and the "add to cart" form.
- Creates a customizable set of Views pages for displaying products located at /products, /products/tags/TAG_ID, and /products/collections/COLLECTION_ID
- Creates a block with links to each Collection page to use as a menu. New collections are added automatically.
- Provides a cart block which shows the amount of items in a user's cart and links to their cart. Will create a font awesome cart image if Font Awesome is installed.
- Product variants and product variant pricing is supported.
- A Shopify theme generator is included to ensure that the transition from your Backdrop site to the Shopify checkout process is seamless.
- Handy links to edit products and other common places on Shopify right from Backdrop.
- Drush integration to retrieve products, sync, and more.
- Products are integrated with Backdrop core search and custom view modes for search are provided.
- Currently the module does not tie orders to Backdrop users, but it's possible using the Shopify API.

This module is in no way an official project of Shopify.

## Installation and Usage

- Install this module using the [official Backdrop CMS instructions](https://backdropcms.org/guide/modules)
- Usage instructions can be [viewed and edited in the Wiki](https://github.com/backdrop-contrib/shopify/wiki).

## Dependencies

- A Shopify subscription is required.
- @todo Pathauto Entity (https://www.drupal.org/project/pathauto_entity) is recommended.

## Issues

 - Bugs and Feature requests should be reported in the [Issue Queue](https://github.com/backdrop-contrib/shopify/issues).

## Current Maintainers

 - [Laryn Kragt Bakker](https://github.com/laryn) - [CEDC.org](https://cedc.org)
 - Collaboration and co-maintainers welcome!

## Credits

 - Ported to BackdropCMS by [Laryn Kragt Bakker](https://github.com/laryn) - [CEDC.org](https://cedc.org)
 - @todo Maintainers on drupal.org include [Bonify, LLC](http://bonify.io).

## License

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
