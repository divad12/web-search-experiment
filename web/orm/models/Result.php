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

class Result extends Doctrine_Record {
  public function setTableDefinition() {
    $this->setTableName('Result');
    $this->hasColumn('ID', 'integer', 4, array(
        'type' => 'integer',
        'length' => 4,
        'unsigned' => false,
        'notnull' => true,
        'primary' => true, 
        'autoincrement' => true,
      )
    );
    $this->hasColumn('Title as title', 'string', 255, array(
        'type' => 'string',
        'length' => 255,
        'fixed' => false,
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
    $this->hasColumn('Description as description', 'string', 255, array(
        'type' => 'string',
        'length' => 255,
        'fixed' => false,
        'notnull' => false,
      )
    );
    $this->hasColumn('Rank as rank', 'integer', 4, array(
        'type' => 'integer',
        'length' => 4,
        'unsigned' => false,
        'notnull' => true,
      )
    );
  }
  
  public function setUp() {
    parent::setUp();
    $this->hasMany('ResultEvent as resultEvents', array(
        'local' => 'ID', 
        'foreign' => 'ResultID'
      )
    );
    $this->hasMany('Event as events', array(
        'local' => 'ResultID', 
        'foreign' => 'EventID', 
        'refClass' => 'Result_Event'
      )
    );
    $this->hasMany('Topic as topics', array(
        'local' => 'ResultID', 
        'foreign' => 'TopicID', 
        'refClass' => 'Result_Topic'
      )
    );
  }
  
}

?>
