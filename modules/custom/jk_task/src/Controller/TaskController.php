<?php

namespace Drupal\jk_task\Controller;

use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\OpenModalDialogCommand;

class TaskAssignController extends ControllerBase{

  public function assignForm($method, $nid) {
    return array(
      '#type' => 'markup',
      '#markup' => $this->t('Hello, World!'),
    );

  }

  public function assignTask($method, $nid_employee, $nid_task) {
    return array(
      '#type' => 'markup',
      '#markup' => $this->t('Hello, World!'),
    );

  }

}