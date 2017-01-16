<?php
/**
 * @file
 * Represents custom pages and logic related to them.
 */

/**
 * Page callback for getting content of webform.
 *
 * @param object $node
 *   Object of node related to webform.
 * @param int $display
 *   Indicates type of display, iframe or not.
 */
function av_webform_embeded_page($node) {
  webform_node_view($node, 'full');
  header_remove('X-Frame-Options');
  $path = drupal_get_path('module', 'av_webform_embeded');
  drupal_add_css($path . '/css/av_webform_embeded_page.css');
  $action = &$node->content['webform']['#form']['#action'];
  $action = url($action, array('absolute' => TRUE));
  return theme_webform_view($node->content);
}

/**
 * Page callback for testing.
 *
 * @param object $node
 *   Object of node related to webform.
 * @param int $display
 *   Indicates type of display, iframe or not.
 *
 * @return string
 *   Rendered webform as HTML.
 */
function av_webform_embeded_test_page($node, $display) {
  $settings = av_webform_embeded_load($node->nid);
  $scripts = av_webform_embeded_generate_code($settings);
  return $scripts;
}

