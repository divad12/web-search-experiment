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

class Result_Topic extends Doctrine_Record {
  public function setTableDefinition() {
    $this->setTableName('Result_Topic');
    $this->hasColumn('ResultID', 'integer', 4, array(
        'primary' => true
      )
    );
    $this->hasColumn('TopicID', 'integer', 4, array(
        'primary' => true
      )
    );
  }
  
}

?>
