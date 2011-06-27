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

class Session extends Doctrine_Record {
  public function setTableDefinition() {
    $this->setTableName('Session');
    $this->hasColumn('ID', 'integer', 4, array(
        'type' => 'integer',
        'length' => 4,
        'unsigned' => false,
        'notnull' => true,
        'primary' => true, 
        'autoincrement' => true,
      )
    );
    $this->hasColumn('ElapsedTime as elapsedTime', 'time', 0, array(
        'type' => 'time',
        'length' => 0,
        'notnull' => false,
      )
    );
  }
  
  public function setUp() {
    parent::setUp();
    $this->hasOne('Topic as topic', array(
        'local' => 'ID', 
        'foreign' => 'TopicID'
      )
    );
    $this->hasOne('User as user', array(
        'local' => 'ID', 
        'foreign' => 'UserID'
      )
    );
    $this->hasMany('Event as events', array(
        'local' => 'ID', 
        'foreign' => 'SessionID'
      )
    );
  }
  
}

?>
