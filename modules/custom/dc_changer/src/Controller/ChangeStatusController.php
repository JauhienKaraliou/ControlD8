<?php

namespace Drupal\dc_changer\Controller;

use Drupal\Core\Controller\ControllerBase;
use Drupal\node\Entity\Node;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\HtmlCommand;
use Drupal\Core\Ajax\RemoveCommand;

class ChangeStatusController extends ControllerBase {
  public function changeStatus($method, $task_status, $nid) {

    $node = Node::load($nid);
    $node->field_date_of_completion['und'][0]['value'] = time();

    if ($task_status == 'done') {
      $status_term = \Drupal::entityTypeManager()
        ->getStorage('taxonomy_term')
        ->loadByProperties(['name' => 'Done']);
    }
    else {
      $status_term = \Drupal::entityTypeManager()
        ->getStorage('taxonomy_term')
        ->loadByProperties(['name' => 'Not done']);
    }

    $node->field_status['und'][0]['tid'] = $status_term->tid;
    $node->save();

    if ($method === 'ajax') {
      $response = new AjaxResponse();
      $response->addCommand(new RemoveCommand('.control-actions-link[data-nid=$nid]'));
      $response->addCommand(new HtmlCommand('.assigned-task-status[data-nid=$nid]', $status_term->name));

      return $response;
    }
  }
}

function _dc_changer_get_statuses() {
  $query = \Drupal::entityQuery('taxonomy_term');
  $query->condition('vid', 'statuses');
  $tids = $query->execute();
  $statuses = \Drupal\taxonomy\Entity\Term::loadMultiple($tids);

//  $vocabulary_statuses = taxonomy_vocabulary_machine_name_load('statuses');
//  $statuses = taxonomy_term_load_multiple(NULL, array('vid' => $vocabulary_statuses->vid));

  $statuses_array = array();
  foreach ($statuses as $status) {
    if ($status->name == 'Not done') {
      $statuses_array['status_not_done'] = $status;
    }
    else if ($status->name == 'Done') {
      $statuses_array['status_done'] = $status;
    }
    else {
      $statuses_array['status_in_progress'] = $status;
    }
  }

  return $statuses_array;
}
