<?php

/**
 * @file
 * Template to display a view as a table.
 *
 * - $title : The title of this group of rows.  May be empty.
 * - $header: An array of header labels keyed by field id.
 * - $caption: The caption for this table. May be empty.
 * - $header_classes: An array of header classes keyed by field id.
 * - $fields: An array of CSS IDs to use for each field id.
 * - $classes: A class or classes to apply to the table, based on settings.
 * - $row_classes: An array of classes to apply to each row, indexed by row
 *   number. This matches the index in $rows.
 * - $rows: An array of row items. Each row is an array of content.
 *   $rows are keyed by row number, fields within rows are keyed by field ID.
 * - $field_classes: An array of classes to apply to each field, indexed by
 *   field id, then row number. This matches the index in $rows.
 * @ingroup views_templates
 */
?>
<table <?php if ($classes) {
    print 'class="' . $classes . '" ';
} ?><?php print $attributes; ?>>
    <?php if (!empty($title) || !empty($caption)) : ?>
        <caption><?php print $caption . $title; ?></caption>
    <?php endif; ?>
    <?php if (!empty($header)) : ?>
        <thead>
        <tr>
            <?php foreach ($header as $field => $label): ?>
                <th <?php if ($header_classes[$field]) {
                    print 'class="' . $header_classes[$field] . '" ';
                } ?> scope="col">
                    <?php print $label; ?>
                </th>
            <?php endforeach; ?>
        </tr>
        </thead>
    <?php endif; ?>
    <tbody>
    <?php foreach ($rows as $row_count => $row):
        $node_at = node_load($row['nid']);
        $node_task = node_load($node_at->field_nid_task['und'][0]['target_id']);
        if (isset($node_task->field_approximate_time['und'][0]['value'])) {
            $approximate_time = $node_task->field_approximate_time['und'][0]['value'];
            $time_difference = ai_time_difference_get_difference($row['nid']);
            $time_difference_in_seconds = ai_time_difference_convert_time_difference_to_seconds($time_difference);
            $approximate_time_in_seconds = __dc_convert_to_seconds_DD_HH_MM_SS($approximate_time);
            $time_difference = $time_difference->format('%D:%H:%I:%S');
            $delay_time = $approximate_time_in_seconds - $time_difference_in_seconds;

            if ($row_classes[$row_count]) {
                if ($time_difference > $approximate_time) {
                    print '<tr class=danger "' . implode(' ', $row_classes[$row_count]) . '">';
                }
                elseif ($approximate_time_in_seconds - $time_difference_in_seconds < 30 * AI_TIME_DIFFERENCE_COUNT_SECONDS_IN_MINUTE) {
                    print '<tr class=warning "' . implode(' ', $row_classes[$row_count]) . '">';
                }
                else {
                    print '<tr class="' . implode(' ', $row_classes[$row_count]) . '">';
                }
            }
        }
        else {
            if ($row_classes[$row_count]) {
                print '<tr class="' . implode(' ', $row_classes[$row_count]) . '">';
            }
        }

        foreach ($row as $field => $content): ?>
            <td <?php if ($field_classes[$field][$row_count]) {
                print 'class="' . $field_classes[$field][$row_count] . '" ';
            } ?><?php print drupal_attributes($field_attributes[$field][$row_count]); ?>>
                <?php
                if ($field == 'field_delay_time' && isset($delay_time) && $delay_time < 0) {
                    print ai_employees_list_convert_second_to_time_format(abs($delay_time));
                }
                else {
                    print $content;
                }
                ?>
            </td>
        <?php endforeach; ?>
        </tr>
    <?php endforeach; ?>
    </tbody>
</table>
