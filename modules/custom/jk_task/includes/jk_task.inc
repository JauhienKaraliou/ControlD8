<?php
/**
 * @file
 * Module for opening assign task form in popup.
 */

/**
 * Ajax modal window callback.
 */
//function ai_assign_task_callback($js = NULL) {
//  ctools_include('node.pages', 'node', '');
//  ctools_include('modal');
//  ctools_include('ajax');
//  _ai_assign_task_include_modal();
//  global $user;
//  $type = 'assigned_tasks';
//  $assigned_task = new stdClass();
//  $assigned_task->uid = $user->uid;
//  $assigned_task->name = $user->name;
//  $assigned_task->type = $type;
//  $assigned_task->language = LANGUAGE_NONE;
//  $assigned_task->field_nid_task[LANGUAGE_NONE][0]['value'] = arg(2);
//  if (!$js) {
//    return drupal_get_form($type . '_node_form', $assigned_task);
//  }
//  $form_state = array(
//    'title' => t('Assign task'),
//    'ajax' => TRUE,
//  );
//  $form_state['build_info']['args'] = array($assigned_task);
//  $form_state['build_info']['args']['short'] = array(TRUE);
//  $output = ctools_modal_form_wrapper($type . '_node_form', $form_state);
//  if (!empty($form_state['executed'])) {
//    $output = array();
//    $output[] = ctools_modal_command_dismiss();
//    $output[] = ajax_command_prepend('section.col-sm-12','<div class="alert alert-block alert-success messages status"><a class="close" data-dismiss="alert" href="#">×</a><h4 class="element-invisible">Status message</h4>Assigned tasks has been created.</div>');
//  }
//
//  drupal_get_messages();
//  print ajax_render($output);
//  drupal_exit();
//}
//
///**
// * Function to set modal window settings.
// */
//function _ai_assign_task_include_modal() {
//  static $added = FALSE;
//  if ($added == FALSE) {
//    $added = TRUE;
//    ctools_include('modal');
//    ctools_include('ajax');
//    ctools_modal_add_js();
//    $popup_style = array(
//      'assign-popup-style' => array(
//        'modalSize' => array(
//          'type' => 'fixed',
//          'width' => 1000,
//          'height' => 600,
//          'addHeight' => 100,
//        ),
//        'modalOptions' => array(
//          'opacity' => (float) 0.8,
//          'background-color' => '#084b57',
//        ),
//        'closeText' => 'dismiss',
//        'loadingText' => 'wait',
//        'animation' => 'fadeIn',
//        'animationSpeed' => 'fast',
//      ),
//    );
//
//    drupal_add_js($popup_style, 'setting');
//  }
//}

/**
 * Callback function. Selects employees list to be returned for pre-rendering.
 */
function jk_assign_task_employees_list_callback($form, $form_state) {
  return $form['field_nid_employee'];
}

/**
 * Function returns a list of employees that didn't do selected task before.
 */
function jk_assign_task_get_employees($task_id) {
  $user = \Drupal::currentUser();
  $options = array();

  $query = \Drupal::database()->select('field_data_field_nid_employee', 'ne');
  $query->distinct();
  $query->innerJoin('field_data_field_nid_task', 'nt', 'ne.entity_id = nt.entity_id');
  $query->fields('ne', array('field_nid_employee_target_id'));
  $query->condition('nt.field_nid_task_target_id', $task_id);
  $employees_nids = $query->execute()->fetchAll();

  $query = \Drupal::database()->select('node', 'n');
  $query->fields('n', array('nid', 'title'));
  $query->condition('type', 'employees');
  $query->condition('n.uid',$user->uid);

  if ($employees_nids) {
    $nids = array();
    foreach ($employees_nids as $employee_nid) {
      $nids[] = $employee_nid->field_nid_employee_target_id;
    }
    $query->condition('nid', $nids, 'NOT IN');
  }
  $employees = $query->execute()->fetchAll();

  if ($employees) {
    foreach ($employees as $employee) {
      $options[$employee->nid] = $employee->title;
    }
  }

  return $options;
}
