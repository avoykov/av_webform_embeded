<?php
/**
 * @file
 * Represents custom forms and logic related to them.
 */

/**
 * Form handler for configuration embeded webform.
 *
 * @param array $form
 *   Array with form definition.
 * @param array $form_state
 *   Array with form state.
 *
 * @return array
 *   Renderable array.
 */
function av_webform_embeded_form($form, &$form_state, $node) {
  $form['#tree'] = TRUE;

  $settings = av_webform_embeded_load($node->nid);
  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Embeded webform configuration:'),
    '#collapsible' => FALSE,
  );

  $form['settings']['display'] = array(
    '#type' => 'radios',
    '#title' => t('Display as'),
    '#description' => t('Choose way for displaying form.'),
    '#options' => array(
      'popup' => t('Popup'),
      'embeded' => t('Embeded'),
    ),
    '#default_value' => !empty($settings->display) ? $settings->display : 'popup',
    '#required' => TRUE,
  );

  $form['settings']['type'] = array(
    '#type' => 'radios',
    '#title' => t('Type'),
    '#description' => t('Choose which way should be used.'),
    '#options' => array(
      'iframe' => t('Iframe'),
      'html' => t('HTML'),
    ),
    '#default_value' => !empty($settings->type) ? $settings->type : 'iframe',
    '#required' => TRUE,
  );

  $form['settings']['height'] = array(
    '#type' => 'textfield',
    '#title' => t('Height'),
    '#description' => t('Enter height of embeded object.'),
    '#default_value' => !empty($settings->height) ? $settings->height : '300px',
  );

  $form['settings']['width'] = array(
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#description' => t('Enter width of embeded object.'),
    '#default_value' => !empty($settings->width) ? $settings->width : '400px',
  );

  $form['settings']['position'] = array(
    '#type' => 'select',
    '#title' => t('Position'),
    '#description' => t('Choose position of embeded object.'),
    '#default_value' => !empty($settings->position) ? $settings->position : 'center',
    '#options' => array(
      'center' => t('Center'),
      'left' => t('Left'),
      'top' => t('Top'),
      'right' => t('Right'),
      'bottom' => t('Bottom'),
    ),
    '#required' => TRUE,
  );
  $parts = !empty($settings->event) ? explode('_', $settings->event) : array();
  $form['settings']['event'] = array(
    '#type' => 'select',
    '#title' => t('Event'),
    '#description' => t('Choose event for displaying embeded object.'),
    '#default_value' => !empty($parts[0]) ? $parts[0] : 'onload',
    '#options' => array(
      'onload' => t('Immediately on page load'),
      'onscroll' => t('On starting scroll'),
      'onbottom' => t('On reaching end of page'),
      'onexit' => t('On to exit page'),
      'delay' => t('After certain time'),
    ),
    '#required' => TRUE,
    '#ajax' => array(
      'wrapper' => 'av-we-event-wrapper',
      'callback' => 'av_we_event_ajax_handler',
      'effect' => 'none',
    ),
  );

  $class = 'av-we-hide';
  $selected_event = NULL;
  if (!empty($form_state['input']['settings']['event'])) {
    $class = $form_state['input']['settings']['event'] == 'delay' ? '' : 'av-we-hide';
  }
  elseif (!empty($parts[0])) {
    $class = $parts[0] == 'delay' ? '' : 'av-we-hide';
  }
  $form['settings']['delay'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($parts[1]) ? $parts[1] : '',
    '#attributes' => array(
      'placeholder' => t('Seconds'),
      'class' => array($class),
    ),
    '#prefix' => '<div id="av-we-event-wrapper">',
    '#suffix' => '</div>',
    '#size' => 6,
    '#required' => TRUE,
  );

  $form['conditions_wrapper'] = array(
    '#type' => 'fieldset',
    '#title' => t('Conditions'),
    '#collapsible' => TRUE,
    '#prefix' => '<div id="av-we-conditions-wrapper">',
    '#suffix' => '</div>',
    '#empty' => t("At this moment not one condition doesn't exists"),
    '#theme' => 'av_we_conditions_table',
    '#header' => array(
      'type' => t('Type'),
      'condition' => t('Condition'),
      'data' => t('Data'),
      'delete' => t('Delete'),
    ),
  );
  if (empty($form_state['conditions_count'])) {
    $ids = array_keys($settings->conditions);
    if (!empty($ids)) {
      $form_state['conditions_count'] = array_combine($ids, $ids);
    }
    else {
      $form_state['conditions_count'] = array(0 => 0);
    }
  }

  foreach ($form_state['conditions_count'] as $i) {
    $form['conditions_wrapper']['conditions'][$i]['type'] = array(
      '#type' => 'select',
      '#default_value' => !empty($settings->conditions[$i]->type) ? $settings->conditions[$i]->type : '',
      '#options' => array(
        'page' => t('Page URL'),
        'referrer' => t('Referrer URL'),
        'frequence' => t('Frequence'),
        'device' => t('Device'),
        'agent' => t('User agent'),
      ),
      '#empty_option' => t('- Select -'),
      '#condition_id' => $i,
      '#ajax' => array(
        'wrapper' => 'av-we-condition-wrapper-' . $i,
        'callback' => 'av_we_condition_type_ajax_handler',
        'effect' => 'none',
      ),
    );
    $selected_type = NULL;
    if (!empty($form_state['input']['conditions_wrapper']['conditions'][$i]['type'])) {
      $selected_type = $form_state['input']['conditions_wrapper']['conditions'][$i]['type'];
    }
    elseif (!empty($settings->conditions[$i]->type)) {
      $selected_type = $settings->conditions[$i]->type;
    }

    $selected_condition = NULL;
    if (!empty($form_state['input']['conditions_wrapper']['conditions'][$i]['condition'])) {
      $selected_condition = $form_state['input']['conditions_wrapper']['conditions'][$i]['condition'];
    }
    elseif (!empty($settings->conditions[$i]->condition)) {
      $selected_condition = $settings->conditions[$i]->condition;
    }

    $result = _get_options_by_condition($selected_type, $selected_condition);
    $form['conditions_wrapper']['conditions'][$i]['condition'] = array(
      '#type' => 'select',
      '#prefix' => '<div id="av-we-condition-wrapper-' . $i . '">',
      '#suffix' => '</div>',
      '#options' => _get_options_by_type($selected_type),
      '#default_value' => !empty($settings->conditions[$i]->condition) ? $settings->conditions[$i]->condition : NULL,
      '#condition_id' => $i,
      '#ajax' => array(
        'wrapper' => 'av-we-type-wrapper-' . $i,
        'callback' => 'av_we_condition_condition_ajax_handler',
        'effect' => 'none',
      ),
    );

    $form['conditions_wrapper']['conditions'][$i]['data'] = array(
      '#type' => 'textfield',
      '#default_value' => !empty($settings->conditions[$i]->data) ? $settings->conditions[$i]->data : NULL,
      '#condition_id' => $i,
      '#attributes' => array(
        'placeholder' => $result['data'],
        'class' => array($result['class']),
      ),
      '#prefix' => '<div id="av-we-type-wrapper-' . $i . '">',
      '#suffix' => '</div>',
    );

    $form['conditions_wrapper']['conditions'][$i]['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Remove'),
      '#name' => 'av_we_delete_' . $i,
      '#condition_id' => $i,
      '#cid' => !empty($settings->condition[$i]->nid) ? $settings->condition[$i]->nid : NULL,
      '#limit_validation_errors' => array(),
      '#submit' => array('av_we_condition_remove_submit'),
      '#ajax' => array(
        'wrapper' => 'av-we-conditions-wrapper',
        'callback' => 'av_we_condition_ajax_handler',
        'effect' => 'none',
      ),
    );
  }

  $form['conditions_wrapper']['add'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
    '#name' => 'av_we_add',
    '#submit' => array('av_we_condition_add_submit'),
    '#limit_validation_errors' => array(),
    '#ajax' => array(
      'wrapper' => 'av-we-conditions-wrapper',
      'callback' => 'av_we_condition_ajax_handler',
      'effect' => 'none',
    ),
  );

  $form['actions'] = array('#type' => 'actions', '#weight' => 50);

  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#validate' => array('av_webform_embeded_form_validate'),
    '#submit' => array('av_webform_embeded_form_submit'),
  );

  $form['actions']['code'] = array(
    '#type' => 'submit',
    '#value' => t('Get code'),
    '#submit' => array('av_webform_embeded_form_code'),
    '#validate' => array('av_webform_embeded_form_validate'),
    '#limit_validation_errors' => array(),
    '#ajax' => array(
      'wrapper' => 'av-webform-embeded',
      'callback' => 'av_webform_embeded_ajax_callback',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  $form['old'] = array(
    '#type' => 'value',
    '#value' => $settings,
  );

  $form['node'] = array(
    '#type' => 'value',
    '#value' => $node,
  );
  $form['script'] = array(
    '#prefix' => '<div id="av-webform-embeded">',
    '#suffix' => '</div>',
    '#type' => 'textarea',
    '#weight' => 51,
    '#attributes' => array(
      'readonly' => 'readonly',
    ),
    '#title' => t('Embeded code:'),
    '#description' => t('You can use this code for inserting to external sites.'),
    '#rows' => 10,
    '#value' => isset($form_state['script']) ? $form_state['script'] : t('Please click to generate button for getting embeded code.'),
  );

  $form['#attached']['css'][] = drupal_get_path('module', 'av_webform_embeded') . '/css/av_webform_embeded_form.css';
  return $form;
}

/**
 * Validate form handler for embeded webform form.
 *
 * @param array $form
 *   Array with form definition.
 * @param array $form_state
 *   Array with form state.
 */
function av_webform_embeded_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $pattern = '/\d+(px|%)/';

  if (!empty($values['settings']['height']) && !preg_match($pattern, $values['settings']['height'])) {
    form_set_error('settings][height', t('Height should be a number with suffix px or %.'));
  }

  if (!empty($values['settings']['height']) && !preg_match($pattern, $values['settings']['width'])) {
    form_set_error('settings][width', t('Width should be a number with suffix px or %.'));
  }
}

/**
 * Submit form handler for embeded webform form.
 *
 * @param array $form
 *   Array with form definition.
 * @param array $form_state
 *   Array with form state.
 */
function av_webform_embeded_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $settings = (object) $values['settings'];
  $settings->nid = $values['node']->nid;
  $settings->id = !empty($values['old']->id) ? $values['old']->id : NULL;
  if (!empty($settings->delay)) {
    $settings->event .= '_' . $settings->delay;
  }
  unset($settings->delay);
  // Process conditions.
  $conditions = array_map(function ($i) {
    unset($i['delete']);
    return $i;
  }, $values['conditions_wrapper']['conditions']);
  $conditions = array_filter($conditions, function ($i) {
    return !empty($i['type']) && !empty($i['condition']);
  });

  unset($settings->actions);
  $settings->conditions = (object) $conditions;
  $result = av_webform_embeded_save($settings);
  if (!empty($result->id)) {
    drupal_set_message(t('Settings was successfully saved.'));
  }
  else {
    drupal_set_message(t('Something happened during saving.'), 'error');
  }
  $form_state['rebuild'] = TRUE;
  $form_state['redirect'] = "node/{$settings->nid}/webform-embeded";
}

/**
 * Ajax handler for embeded webform form.
 *
 * @param array $form
 *   Array with form definition.
 * @param array $form_state
 *   Array with form state.
 *
 * @return array
 *   Part of form which should be rendered.
 */
function av_webform_embeded_ajax_callback($form, &$form_state) {
  return $form['script'];
}

/**
 * Submit form handler for getting code.
 *
 * @param array $form
 *   Array with form definition.
 * @param array $form_state
 *   Array with form state.
 */
function av_webform_embeded_form_code($form, &$form_state) {
  $values = $form_state['values'];
  $settings = (object) $values['settings'];
  $settings->nid = $values['node']->nid;
  if (!empty($settings->delay)) {
    $settings->event .= '_' . $settings->delay;
  }
  unset($settings->delay);
  // Process conditions.
  $conditions = array_map(function ($i) {
    unset($i['delete']);
    return $i;
  }, $values['conditions_wrapper']['conditions']);
  $conditions = array_filter($conditions, function ($i) {
    return !empty($i['type']) && !empty($i['condition']);
  });

  unset($settings->actions);
  $settings->conditions = (object) $conditions;
  $form_state['script'] = av_webform_embeded_generate_code($settings);
  $form_state['rebuild'] = TRUE;
}

/**
 * Submit handler for adding configs.
 *
 * @param array $form
 *   Array with form definition.
 * @param array $form_state
 *   Array with form state.
 */
function av_we_condition_add_submit($form, &$form_state) {
  $max = max(array_values($form_state['conditions_count']));
  $form_state['conditions_count'][$max + 1] = $max + 1;
  $form_state['rebuild'] = TRUE;
}

/**
 * Submit handler for adding configs.
 *
 * @param array $form
 *   Array with form definition.
 * @param array $form_state
 *   Array with form state.
 */
function av_we_condition_remove_submit($form, &$form_state) {
  $elem = $form_state['triggering_element'];
  unset($form_state['conditions_count'][$elem['#condition_id']]);
  $form_state['rebuild'] = TRUE;
}

/**
 * Ajax handler for getting part of form.
 *
 * @param array $form
 *   Array with form definition.
 * @param array $form_state
 *   Array with form state.
 *
 * @return array
 *   Part of form which should be rerendered.
 */
function av_we_condition_ajax_handler($form, &$form_state) {
  $elem = $form_state['triggering_element'];
  $parents = $elem['#parents'];
  switch (substr($elem['#name'], 0, 12)) {
    case 'av_we_add':
      array_pop($parents);
      return drupal_array_get_nested_value($form, $parents);

    case 'av_we_delete':
      $parents = array_slice($parents, 0, count($parents) - 3);
      return drupal_array_get_nested_value($form, $parents);
  }
  return $form;
}

/**
 * Ajax handler for getting part of form.
 *
 * @param array $form
 *   Array with form definition.
 * @param array $form_state
 *   Array with form state.
 *
 * @return array
 *   Part of form which should be rerendered.
 */
function av_we_condition_type_ajax_handler($form, &$form_state) {
  $elem = $form_state['triggering_element'];
  $parents = $elem['#parents'];
  $parents = array_slice($parents, 0, count($parents) - 1);
  $parents[] = 'condition';
  return drupal_array_get_nested_value($form, $parents);

}

/**
 * Ajax handler for getting part of form.
 *
 * @param array $form
 *   Array with form definition.
 * @param array $form_state
 *   Array with form state.
 *
 * @return array
 *   Part of form which should be rerendered.
 */
function av_we_condition_condition_ajax_handler($form, &$form_state) {
  $elem = $form_state['triggering_element'];
  $parents = $elem['#parents'];
  $parents = array_slice($parents, 0, count($parents) - 1);
  $parents[] = 'data';
  return drupal_array_get_nested_value($form, $parents);
}

/**
 * Ajax handler for getting part of form.
 *
 * @param array $form
 *   Array with form definition.
 * @param array $form_state
 *   Array with form state.
 *
 * @return array
 *   Part of form which should be rerendered.
 */
function av_we_event_ajax_handler($form, &$form_state) {
  $elem = $form_state['triggering_element'];
  $parents = $elem['#parents'];
  $parents = array_slice($parents, 0, count($parents) - 1);
  $parents[] = 'delay';
  return drupal_array_get_nested_value($form, $parents);
}

/**
 * Helper function for getting derived options.
 *
 * @param string $type
 *   Select type of condition.
 *
 * @return array
 *   Options of condition select.
 */
function _get_options_by_type($type) {
  $condition_options = array(
    'page' => array(
      '' => t('- Select -'),
      'contains' => t("contains"),
      'not_contains' => t("doesn't contains"),
      'is' => t("is"),
      'isnt' => t("isn't"),
    ),
    'referrer' => array(
      '' => t('- Select -'),
      'contains' => t("contains"),
      'not_contains' => t("doesn't contains"),
      'is' => t("is"),
      'isnt' => t("isn't"),
    ),
    'frequence' => array(
      '' => t('- Select -'),
      'always' => t("Everytime"),
      'session' => t("Once per session"),
      'once' => t("Once"),
      'once_per' => t("Once every"),
      'max' => t("Maximum time to show"),
    ),
    'device' => array(
      '' => t('- Select -'),
      'is' => t("is"),
    ),
    'agent' => array(
      '' => t('- Select -'),
      'contains' => t("contains"),
      'not_contains' => t("doesn't contains"),
      'is' => t("is"),
      'isnt' => t("isn't"),
    ),
  );

  return !empty($condition_options[$type]) ? $condition_options[$type] : array('' => t('- Select -'));
}

/**
 * Helper function for getting derived options.
 *
 * @param string $type
 *   Select type of condition.
 *
 * @return array
 *   Options of condition select.
 */
function _get_options_by_condition($type, $condition) {
  $condition_options = array(
    'page' => array(
      'contains' => array(
        'data' => t('Part of url'),
        'class' => '',
      ),
      'not_contains' => array(
        'data' => t('Part of url'),
        'class' => '',
      ),
      'is' => array(
        'data' => t('URL'),
        'class' => '',
      ),
      'isnt' => array(
        'data' => t('URL'),
        'class' => '',
      ),
    ),
    'referrer' => array(
      'contains' => array(
        'data' => t('Part of url'),
        'class' => '',
      ),
      'not_contains' => array(
        'data' => t('Part of url'),
        'class' => '',
      ),
      'is' => array(
        'data' => t('URL'),
        'class' => '',
      ),
      'isnt' => array(
        'data' => t('URL'),
        'class' => '',
      ),
    ),
    'frequence' => array(
      'always' => array(
        'data' => t('Will be executed always'),
        'class' => 'av-we-hide',
      ),
      'session' => array(
        'data' => t('Count'),
        'class' => '',
      ),
      'once' => array(
        'data' => t('Will be executed once'),
        'class' => 'av-we-hide',
      ),
      'once_per' => array(
        'data' => t('Minute'),
        'class' => '',
      ),
      'max' => array(
        'data' => t('Count'),
        'class' => '',
      ),
    ),
    'device' => array(
      'is' => array(
        'data' => t('Mobile'),
        'class' => '',
      ),
    ),
    'agent' => array(
      'contains' => array(
        'data' => t('Part of user agent'),
        'class' => '',
      ),
      'not_contains' => array(
        'data' => t('Part of user agent'),
        'class' => '',
      ),
      'is' => array(
        'data' => t('User agent'),
        'class' => '',
      ),
      'isnt' => array(
        'data' => t('User agent'),
        'class' => '',
      ),
    ),
  );

  return !empty($condition_options[$type][$condition]) ? $condition_options[$type][$condition] : array(
    'data' => '',
    'class' => 'av-we-hide',
  );
}
