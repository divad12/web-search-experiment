<?php
/**
 * "Visual Paradigm: DO NOT MODIFY THIS FILE!"
 * 
 * This is an automatic generated file. It will be regenerated every time 
 * you generate persistence class.
 * 
 * Modifying its content may cause the program not work, or your work may lost.
 */

/**
 * Licensee: Mack Duan
 * License Type: Evaluation
 */

class Event extends Doctrine_Record {
  public function setTableDefinition() {
    $this->setTableName('Event');
    $this->hasColumn('ID', 'integer', 4, array(
        'type' => 'integer',
        'length' => 4,
        'unsigned' => false,
        'notnull' => true,
        'primary' => true, 
        'autoincrement' => true,
      )
    );
    $this->hasColumn('SessionID', 'integer', 4, array(
        'type' => 'integer',
        'length' => 4,
        'unsigned' => false,
        'notnull' => true,
      )
    );
    $this->hasColumn('Time as time', 'time', 0, array(
        'type' => 'time',
        'length' => 0,
        'notnull' => true,
      )
    );
    $this->hasColumn('Seq as seq', 'integer', 4, array(
        'type' => 'integer',
        'length' => 4,
        'unsigned' => false,
        'notnull' => true,
      )
    );
    $this->hasColumn('Name as name', 'string', 255, array(
        'type' => 'string',
        'length' => 255,
        'fixed' => false,
        'notnull' => false,
      )
    );
    $this->hasColumn('Query as query', 'string', 255, array(
        'type' => 'string',
        'length' => 255,
        'fixed' => false,
        'notnull' => false,
      )
    );
    $this->hasColumn('Direction as direction', 'integer', 4, array(
        'type' => 'integer',
        'length' => 4,
        'unsigned' => false,
        'notnull' => false,
      )
    );
    $this->hasColumn('ResultID', 'integer', 4, array(
        'type' => 'integer',
        'length' => 4,
        'unsigned' => false,
        'notnull' => false,
      )
    );
    $this->hasColumn('Url as url', 'string', 255, array(
        'type' => 'string',
        'length' => 255,
        'fixed' => false,
        'notnull' => false,
      )
    );
    $this->hasColumn('Discriminator', 'string', 255);
    
    $this->setSubclasses(array(
        'SearchEvent' => array('Discriminator' => 'SearchEvent'),
        'PageEvent' => array('Discriminator' => 'PageEvent'),
        'ResultEvent' => array('Discriminator' => 'ResultEvent'),
        'LoadEvent' => array('Discriminator' => 'LoadEvent'),
        'FocusEvent' => array('Discriminator' => 'FocusEvent'),
        'UnfocusEvent' => array('Discriminator' => 'UnfocusEvent'),
      )
    );
  }
  
  public function setUp() {
    parent::setUp();
    $this->hasOne('Session as session', array(
        'local' => 'SessionID', 
        'foreign' => 'ID'
      )
    );
    $this->hasMany('Result as results', array(
        'local' => 'EventID', 
        'foreign' => 'ResultID', 
        'refClass' => 'Result_Event'
      )
    );
  }
  
}

?>
