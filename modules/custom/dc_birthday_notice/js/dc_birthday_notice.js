jQuery(document).ready(function () {
    jQuery('.notice').prependTo('.front');
    jQuery('.notice').hide();
    jQuery('.notice').show(200);
    setTimeout(function () {
        jQuery('.notice').hide(200);
    },10000);
    jQuery('.notice').hover(function () {
        jQuery(this).prepend('<div class="close_notice"></div>');
        jQuery('.close_notice').html('x');
        jQuery('.close_notice').on('click', function () {
            jQuery('.notice').hide(200);
        })
    },function () {
        jQuery('.close_notice').remove();
    });
});
