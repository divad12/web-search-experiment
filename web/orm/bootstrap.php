<?php
// Setup Doctrine
define('DOCTRINE_PATH', dirname(__FILE__) . '/lib/doctrine/lib');
define('MODELS_PATH', dirname(__FILE__) . '/models');
define('DSN', 'mysql://root:mackduan@localhost/web_logging');

require_once(DOCTRINE_PATH . '/Doctrine.php');

spl_autoload_register(array('Doctrine', 'autoload'));
spl_autoload_register(array('Doctrine', 'modelsAutoload'));

$manager = Doctrine_Manager::getInstance();
$manager->setAttribute(Doctrine::ATTR_QUOTE_IDENTIFIER, true);

Doctrine_Core::setModelsDirectory(MODELS_PATH);

// open connection sample code
// $conn = Doctrine_Manager::connection(DSN);
?>
