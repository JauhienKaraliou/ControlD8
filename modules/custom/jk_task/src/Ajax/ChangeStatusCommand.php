<?php

namespace Drupal\jk_task\Ajax;

use Drupal\Core\Ajax\CommandInterface;

class ChangeStatusCommand implements CommandInterface {

  public function render() {
    return array(
      'command' => 'change_status',
  );
  }
}