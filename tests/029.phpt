--TEST--
Check for couchbase_view
--SKIPIF--
<?php include "skipif.inc" ?>
--FILE--
<?php
include "couchbase.inc";
$handle = couchbase_connect(COUCHBASE_CONFIG_HOST, COUCHBASE_CONFIG_USER, COUCHBASE_CONFIG_PASSWD, COUCHBASE_CONFIG_BUCKET);

$contents = array(
     "foo" => "dummy",
     "bar" => "dummy",
);

couchbase_set_multi($handle, $contents);

$result = couchbase_view($handle, "_all_docs", "");
foreach ($result["rows"] as $key => $value) {
    print_r($value);
}

$handle = new Couchbase(COUCHBASE_CONFIG_HOST, COUCHBASE_CONFIG_USER, COUCHBASE_CONFIG_PASSWD, COUCHBASE_CONFIG_BUCKET);
$result = $handle->view("_all_docs", "");
foreach ($result["rows"] as $key => $value) {
    print_r($value);
}

$handle->flush($handle);
?>
--EXPECTF--
