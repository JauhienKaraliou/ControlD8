<?php
/**
 * @file
 * Module for listing existing employees and their task statistics.
 */
define('AI_EMPLOYEES_LIST_SECONDS_IN_MINUTE', '60');
define('AI_EMPLOYEES_LIST_SECONDS_IN_HOUR', '3600');
define('AI_EMPLOYEES_LIST_SECONDS_IN_DAY', '86400');

/**
 * Function to get employees list with tasks statistics.
 */
function ai_employees_list_get_list($fired = NULL) {
  $employees_nodes = node_load_multiple(_ai_employees_list_get_nid_employees_by_user_sections());

  $sections_vocabulary = taxonomy_vocabulary_machine_name_load('sections');
  $sections = taxonomy_term_load_multiple(NULL, array('vid' => $sections_vocabulary->vid));
  $statuses = _dc_changer_get_statuses();

  $employees_list = array();
  foreach ($employees_nodes as $employee_node) {
    foreach ($sections as $section) {
      $section_uri = '';
      if (isset($section->field_logo_section['und'][0]['uri'])) {
        $section_uri = $section->field_logo_section['und'][0]['uri'];
      }
      if (isset($employee_node->field_sections_employees[LANGUAGE_NONE])) {
        foreach ($employee_node->field_sections_employees[LANGUAGE_NONE] as $field_sections_employee) {
          if ($section->tid == $field_sections_employee['target_id']) {
            $count_tasks = _ai_employees_list_get_count_tasks_by_section_id($section->tid);
            $done = _ai_employees_list_get_tasks_quantity($employee_node->nid, $statuses['status_done']->tid, $section->tid);
            $not_done = _ai_employees_list_get_tasks_quantity($employee_node->nid, $statuses['status_not_done']->tid, $section->tid);

            $spent_time = ai_employees_list_get_spent_time_list($employee_node->nid);
            if ($fired && $employee_node->status != 1) {
              $employees_list[$employee_node->nid][] = array(
                'nid' => $employee_node->nid,
                'name_section' => $section->name,
                'uri_picture_section' => $section_uri,
                'status' => $employee_node->status,
                'name' => l(check_plain($employee_node->title), 'node/' . $employee_node->nid, array()),
                'done' => $done,
                'not_done' => $not_done,
                'in_progress' => _ai_employees_list_get_tasks_quantity($employee_node->nid, $statuses['status_in_progress']->tid, $section->tid),
                'all' => $count_tasks,
                'percentage' => !empty($count_tasks) ? number_format($done * 100 / $count_tasks) : 0,
                'percentage_not_done' => !empty($count_tasks) ? number_format($not_done * 100 / $count_tasks) : 0,
                'spent_time' => $spent_time['time'],
              );
            }
            else if (!$fired && $employee_node->status == 1) {
              $employees_list[$employee_node->nid][] = array(
                'nid' => $employee_node->nid,
                'name_section' => $section->name,
                'uri_picture_section' => $section_uri,
                'status' => $employee_node->status,
                'name' => l(check_plain($employee_node->title), 'node/' . $employee_node->nid, array()),
                'done' => $done,
                'not_done' => $not_done,
                'in_progress' => _ai_employees_list_get_tasks_quantity($employee_node->nid, $statuses['status_in_progress']->tid, $section->tid),
                'all' => $count_tasks,
                'percentage' => !empty($count_tasks) ? number_format($done * 100 / $count_tasks) : 0,
                'percentage_not_done' => !empty($count_tasks) ? number_format($not_done * 100 / $count_tasks) : 0,
                'spent_time' => $spent_time['time'],
              );
            }
          }
        }
      }
    }
  }

  return $employees_list;
}

/**
 * Query to DB. Return List of spent time.
 */
function ai_employees_list_get_spent_time_list($employee_target_id) {
  $query = db_select('field_data_field_spent_time', 'fdfst');
  $query->innerJoin('field_data_field_nid_employee', 'fdfne', 'fdfne.entity_id = fdfst.entity_id');
  $query->fields('fdfst', array('field_spent_time_value'));
  $query->condition('fdfne.field_nid_employee_target_id', $employee_target_id);

  return ai_employees_list_sum_spent_time_list($query->execute()->fetchall());
}

/**
 * Sum spent time employees
 */
function ai_employees_list_sum_spent_time_list($result) {
  $result['time'] = 0;
  foreach ($result as $item) {
    if (isset($item->field_spent_time_value)) {
      $time = explode(':', $item->field_spent_time_value);
      if (isset($time[0]) && isset($time[1]) && isset($time[2]) && isset($time[3])) {
        $item->field_spent_time_value = $time[0] * AI_EMPLOYEES_LIST_SECONDS_IN_DAY + $time[1] * AI_EMPLOYEES_LIST_SECONDS_IN_HOUR + $time[2] * AI_EMPLOYEES_LIST_SECONDS_IN_MINUTE + $time[3];
      }
      $result['time'] += $item->field_spent_time_value;
    }
  }
  $result['time'] = ai_employees_list_convert_second_to_time_format($result['time']);

  return $result;
}

/**
 * Format second to date format D Day(s) HH : MM : SS
 */
function ai_employees_list_convert_second_to_time_format($allSeconds) {
  $days = floor($allSeconds / AI_EMPLOYEES_LIST_SECONDS_IN_DAY);
  $hours = floor(($allSeconds - $days * AI_EMPLOYEES_LIST_SECONDS_IN_DAY) / AI_EMPLOYEES_LIST_SECONDS_IN_HOUR);
  $minutes = floor(($allSeconds - ($days * AI_EMPLOYEES_LIST_SECONDS_IN_DAY + $hours * AI_EMPLOYEES_LIST_SECONDS_IN_HOUR)) / AI_EMPLOYEES_LIST_SECONDS_IN_MINUTE);
  $seconds = $allSeconds - ($days * AI_EMPLOYEES_LIST_SECONDS_IN_DAY + $hours * AI_EMPLOYEES_LIST_SECONDS_IN_HOUR + $minutes * AI_EMPLOYEES_LIST_SECONDS_IN_MINUTE);

  if ($days > 1 || $days == 0) {
    $days .= ' Days ';
  }
  else {
    $days .= ' Day ';
  }

  if ($hours < 10) {
    $hours = '0' . $hours;
  }

  if ($minutes < 10) {
    $minutes = '0' . $minutes;
  }

  if ($seconds < 10) {
    $seconds = '0' . $seconds;
  }

  $formattedTime = $days . $hours . ':' . $minutes . ':' . $seconds;

  return $formattedTime;
}

/**
 * Function to get all employees or tasks, depending on node $type.
 */
function ai_employees_list_get_employees_or_tasks($type) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node');
  $query->propertyCondition('type', $type);
  $result = $query->execute();
  return $result['node'];
}

function _ai_employees_list_get_nid_employees_by_user_sections() {
  $user_sections = _ai_employees_list_get_user_sections();

  $query = db_select('node', 'n');
  $query->fields('n', array('nid'));
  $query->fields('fdfse', array('field_sections_employees_target_id'));
  $query->condition('type', 'employees');
  $query->innerJoin('field_data_field_sections_employees', 'fdfse', 'n.nid = fdfse.entity_id');
  $result = $query->execute()->fetchAll();

  $nid_employees = array();
  foreach ($result as $item) {
    foreach ($user_sections as $user_section) {
      if ($item->field_sections_employees_target_id == $user_section->tid) {
        $nid_employees[$item->nid] = $item->nid;
      }
    }
  }
  return $nid_employees;
}

function _ai_employees_list_get_user_sections() {
  global $user;

  $query = db_select('taxonomy_term_data', 'ttd');
  $query->fields('ttd', array('tid'));
  $query->innerJoin('field_data_field_sections_user', 'fdfsu', 'ttd.tid = fdfsu.field_sections_user_target_id');
  $query->condition('fdfsu.entity_id', $user->uid);
  $tid_sections = $query->execute()->fetchAll();

  $user_sections = array();
  foreach ($tid_sections as $tid_section) {
    $user_sections[] = taxonomy_term_load($tid_section->tid);
  }

  return $user_sections;
}

/**
 *
 */
function _ai_employees_list_get_count_tasks_by_section_id($section_id) {
  $query = db_select('node', 'n');
  $query->innerJoin('field_data_field_section_task', 'fdfst', 'n.nid = fdfst.entity_id');
  $query->condition('type', 'tasks');
  $query->condition('fdfst.field_section_task_target_id', $section_id);
  $query->addExpression('COUNT(*)');
  $result = $query->execute()->fetchField();

  return $result;
}

/**
 * Function to get done or in progress tasks quantity depending on status.
 */
function _ai_employees_list_get_tasks_quantity($employee_id, $status, $section_id) {
  $query = db_select('node', 'n');

  $query->innerJoin('field_data_field_nid_employee', 'fdfne', 'n.nid = fdfne.entity_id');
  $query->innerJoin('field_data_field_status', 'fdfs', 'n.nid = fdfs.entity_id');
  $query->innerJoin('field_data_field_nid_task', 'fdfnt', 'n.nid = fdfnt.entity_id');
  $query->innerJoin('field_data_field_section_task', 'fdfst', 'fdfnt.field_nid_task_target_id = fdfst.entity_id');

  $query->condition('type', 'assigned_tasks');
  $query->condition('fdfne.field_nid_employee_target_id', $employee_id);
  $query->condition('fdfs.field_status_tid', $status);
  $query->condition('fdfst.field_section_task_target_id', $section_id);

  $query->addExpression('COUNT(*)');
  $result = $query->execute()->fetchField();

  return $result;
}

/**
 * Create table header and rows.
 */
function ai_employees_list_prepare_list($lists) {
  $header = array(
    'Employee',
    'Section',
    'In Progress',
    'Done',
    'Not Done',
    'Total',
    'Employee\'s progress',
    'Total spent time'
  );

  $rows = array();
  foreach ($lists as $list) {
    foreach ($list as $param) {
      $section_logo = '';
      if (!empty($param['uri_picture_section'])) {
        $section_logo = _ai_employees_list_render_picture_for_employees_progress_table($param['uri_picture_section']);
      }

      if (!isset($rows[$param['nid']])) {
        $rows[$param['nid']] = array(
          'data' => array(
            'name' => $param['name'],
            'name_section' => $param['name_section'] . $section_logo,
            'in_progress' => check_plain($param['in_progress']),
            'done' => check_plain($param['done']),
            'not_done' => check_plain($param['not_done']),
            'all' => check_plain($param['all']),
            'cool_progressbar' => theme('cool_progressbar', array(
              'percentage' => check_plain($param['percentage']),
              'percentage_not_done' => check_plain($param['percentage_not_done'])
            )),
            'spent_time' => check_plain($param['spent_time']),
          ),
        );
      }
      else {
        $rows[$param['nid']]['data']['name_section'] .= theme('table_cell', array('value' => check_plain($param['name_section']) . $section_logo));
        $rows[$param['nid']]['data']['in_progress'] .= theme('table_cell', array('value' => check_plain($param['in_progress'])));
        $rows[$param['nid']]['data']['done'] .= theme('table_cell', array('value' => check_plain($param['done'])));
        $rows[$param['nid']]['data']['not_done'] .= theme('table_cell', array('value' => check_plain($param['not_done'])));
        $rows[$param['nid']]['data']['all'] .= theme('table_cell', array('value' => check_plain($param['all'])));
        $rows[$param['nid']]['data']['cool_progressbar'] .= theme('cool_progressbar', array(
          'percentage' => check_plain($param['percentage']),
          'percentage_not_done' => check_plain($param['percentage_not_done'])
        ));
      }
    }
  }
  $table_settings = array('header' => $header, 'rows' => $rows);
  return $table_settings;
}

/**
 * Create table header and rows.
 */
function ai_employees_list_prepare_list_who_leave($lists) {
  $header = array(
    'Employee',
    'Section',
    'Done',
    'Total',
    'Employee\'s progress',
    'Total spent time'
  );
  $rows = array();
  foreach ($lists as $list) {
    foreach ($list as $param) {
      $section_logo = '';
      if (!empty($param['uri_picture_section'])) {
        $section_logo = _ai_employees_list_render_picture_for_employees_progress_table($param['uri_picture_section']);
      }

      if (!isset($rows[$param['nid']])) {
        $rows[$param['nid']] = array(
          'data' => array(
            'name' => $param['name'],
            'name_section' => $param['name_section'] . $section_logo,
            'done' => check_plain($param['done']),
            'all' => check_plain($param['all']),
            'cool_progressbar' => theme('cool_progressbar', array(
              'percentage' => check_plain($param['percentage']),
              'percentage_not_done' => check_plain($param['percentage_not_done'])
            )),
            'spent_time' => check_plain($param['spent_time']),
          ),
        );
      }
      else {
        $rows[$param['nid']]['data']['name_section'] .= theme('table_cell', array('value' => check_plain($param['name_section']) . $section_logo));
        $rows[$param['nid']]['data']['done'] .= theme('table_cell', array('value' => check_plain($param['done'])));
        $rows[$param['nid']]['data']['all'] .= theme('table_cell', array('value' => check_plain($param['all'])));
        $rows[$param['nid']]['data']['cool_progressbar'] .= theme('cool_progressbar', array(
          'percentage' => check_plain($param['percentage']),
          'percentage_not_done' => check_plain($param['percentage_not_done'])
        ));
      }
    }
  }
  $table_settings = array('header' => $header, 'rows' => $rows);
  return $table_settings;
}

/**
 * Render image for employees progress table
 */
function _ai_employees_list_render_picture_for_employees_progress_table($uri) {
  return $uri = theme('image', array(
    'width' => 25,
    'height' => 25,
    'path' => file_create_url($uri),
    'attributes' => array('style' => 'display:inline;margin-left:4px;'),
  ));
}
