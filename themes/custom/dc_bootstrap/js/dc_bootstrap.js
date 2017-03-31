(function ($) {
    Drupal.behaviors.dc_autocomplete_assigned_behaviors = {
        attach: function (context, settings) {
            jQuery('.assign_done .task_box_assign span, .table_assign_done span.done').html('DONE');
            jQuery('.assign_in_progress .task_box_assign span, .table_assign_in_progress span.done').html('IN PROGRESS');
            jQuery('.assign_not_done .task_box_assign span, .table_assign_not_done span.done').html('NOT DONE');

            jQuery('.list_mode_panel', context).once('hide_list', function () {
                jQuery(this).hide();
            });

            jQuery('.table_mode').on('click', function () {
                jQuery('.list_mode_panel').hide();
                jQuery('.table_mode_panel').show();
            });

            jQuery('.list_mode').on('click', function () {
                jQuery('.list_mode_panel').show();
                jQuery('.table_mode_panel').hide();
            });
        }
    };
}(jQuery));