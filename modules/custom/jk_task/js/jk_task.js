(function ($) {
    Drupal.ajax.prototype.commands.change_status = function (ajax, response, status) {
        var parents = jQuery(ajax.element).parents();
        if (jQuery(parents[3]).hasClass('table-task')) {
            jQuery(parents[3]).addClass('table_assign_in_progress');
            jQuery(parents[0]).html('IN PROGRESS');
        } else {
            jQuery(parents[3]).addClass('assign_in_progress');
            jQuery(parents[0]).html('IN PROGRESS');
        }
    }
}(jQuery));