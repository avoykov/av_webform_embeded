<?php
/**
 * @file
 * Represents template for rendering embeded code.
 *
 * Variables:
 *  $display - Indicates way for displaying: iframe or html.
 *  $height - Height of popup.
 *  $width - Width of popup.
 *  $position - Position of popup.
 *  $event - Event when popup should be shown.
 *  $js_path - Absolute path to js file.
 *  $css_path - Absolute path to css file.
 *  $src - Url for getting content.
 */
?>
<link href="<?php print $css_path ?>" rel="stylesheet" type="text/css">
<script type="text/javascript" src="<?php print $js_path ?>"></script>
<script type="text/javascript" name="av-we-init">
  avWebformEmbeded.init({
    display: "<?php print $display ?>",
    type: "<?php print $type ?>",
    height: "<?php print $height ?>",
    width: "<?php print $width ?>",
    position: "<?php print $position ?>",
    event: "<?php print $event ?>",
    src: '<?php echo $src ?>',
    conditions: '<?php echo $conditions ?>',
    hash: '<?php echo $hash ?>',
    delay: '<?php echo $delay ?>'
  });
  document.addEventListener("DOMContentLoaded", function(event) {
    avWebformEmbeded.show();
  });
</script>
