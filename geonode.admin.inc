<?php

/**
 * Form builder function for module settings.
 */
function geonode_admin_settings() {
  $form = array();

  $plugins = array('<none>');
  $tplugins = module_invoke_all('geonode_plugin', 'name');
  foreach ($tplugins as $plugin) {
    $title = module_invoke("geonode_{$plugin}", 'geonode_plugin', 'title');
    $plugins[$plugin] = $title ? $title : ucfirst($plugin);
  }
  $form['plugin'] = array(
    '#type' => 'select',
    '#title' => t('Plugin'),
    '#description' => t('Select map plugin to use.'),
    '#default_value' => variable_get('geonode_plugin', ''),
    '#options' => $plugins,
  );

  $nodes = array();
  $tnodes = explode('|', variable_get('geonode_node_types', 'page'));
  foreach ($tnodes as $type) {
    $nodes[$type] = $type;
  }
  $form['node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Content types'),
    '#default_value' => $nodes,
    '#options' => array_map('check_plain', node_get_types('names')),
    '#description' => t('Select content types to use the node geo location.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Implementation of hook_submit().
 */
function geonode_admin_settings_submit(&$form, &$form_state) {
  variable_set('geonode_plugin', $form_state['values']['plugin']);

  $nodes = array();
  foreach ($form_state['values']['node_types'] as $type => $select) {
    if ($select) $nodes[] = $type;
  }
  variable_set('geonode_node_types', implode('|', $nodes));
  drupal_set_message(t('The configuration options have been saved.'));
}

/**
 * Form builder function for plaugin settings.
 */
function geonode_admin_plugin_settings() {
  $plugin = variable_get('geonode_plugin', '');
  if (!$plugin) {
    $form['error'] = array('#value' => t('The plugin cannot be configured. Please visit this <a href="!url">page</a> and select the plugin.', array('!url' => url('admin/settings/geonode'))));
    return $form;
  }

  $form = module_invoke("geonode_{$plugin}", 'geonode_plugin', 'settings_form');
  if (!$form) $form = array('empty' => array('#value' => t('The selected plugin has no settings')));
  elseif (!$form['submit']) $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  return $form;
}

/**
 * Implementation of hook_submit().
 */
function geonode_admin_plugin_settings_submit(&$form, &$form_state) {
  $plugin = variable_get('geonode_plugin', '');
  $form = module_invoke("geonode_{$plugin}", 'geonode_plugin', 'settings_form_submit', $form_state['values']);
  drupal_set_message(t('The configuration options has been saved.'));
}