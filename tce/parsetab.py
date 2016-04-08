
# parsetab.py
# This file is automatically generated. Do not edit.
_tabversion = '3.2'

_lr_method = 'SLR'

_lr_signature = '\xb0\x0ff\xac\n\x1f\x05c\xc01\x1a \x19W\xc2u'
    
_lr_action_items = {'STRUCT':([0,5,12,14,17,18,],[2,2,-5,-7,-6,-4,]),'DICTIONARY':([0,5,12,14,17,18,],[6,6,-5,-7,-6,-4,]),'SEQUENCE':([0,5,12,14,17,18,],[11,11,-5,-7,-6,-4,]),')':([38,46,48,49,51,53,],[-23,-16,50,-15,-16,-17,]),'(':([42,],[46,]),',':([22,23,38,49,],[-24,30,-23,51,]),'>':([22,25,40,],[-24,35,45,]),'INTERFACE':([0,5,12,14,17,18,],[10,10,-5,-7,-6,-4,]),'{':([13,19,],[21,24,]),';':([1,3,8,9,27,38,39,41,44,47,50,],[12,14,17,18,37,-23,-19,-11,-9,-10,52,]),'IDENTIFIER':([2,10,16,20,21,22,24,26,28,30,32,33,34,35,37,45,46,51,52,],[13,19,22,22,22,-24,22,22,38,22,42,-18,22,44,-22,47,22,22,-14,]),'}':([26,29,31,34,36,37,43,52,],[-20,39,41,-12,-21,-22,-13,-14,]),'<':([6,11,],[16,20,]),'$end':([0,4,5,7,12,14,15,17,18,],[-8,-1,-2,0,-5,-7,-3,-6,-4,]),}

_lr_action = { }
for _k, _v in _lr_action_items.items():
   for _x,_y in zip(_v[0],_v[1]):
      if not _x in _lr_action:  _lr_action[_x] = { }
      _lr_action[_x][_k] = _y
del _lr_action_items

_lr_goto_items = {'operatemembers':([24,34,],[31,43,]),'interface_def':([0,5,],[1,1,]),'dictionary_def':([0,5,],[3,3,]),'definations':([0,5,],[4,15,]),'defination':([0,5,],[5,5,]),'type_id':([21,26,46,51,],[27,27,49,49,]),'start':([0,],[7,]),'datamembers':([21,26,],[29,36,]),'sequence_def':([0,5,],[8,8,]),'struct_def':([0,5,],[9,9,]),'callreturn':([24,34,],[32,32,]),'datamember':([21,26,],[26,26,]),'operateparams':([46,51,],[48,53,]),'type':([16,20,21,24,26,30,34,46,51,],[23,25,28,33,28,40,33,28,28,]),'operatemember':([24,34,],[34,34,]),}

_lr_goto = { }
for _k, _v in _lr_goto_items.items():
   for _x,_y in zip(_v[0],_v[1]):
       if not _x in _lr_goto: _lr_goto[_x] = { }
       _lr_goto[_x][_k] = _y
del _lr_goto_items
_lr_productions = [
  ("S' -> start","S'",1,None,None,None),
  ('start -> definations','start',1,'p_start','/root/workspace/test_tce2/src/tce/code/mylex.py',98),
  ('definations -> defination','definations',1,'p_definations','/root/workspace/test_tce2/src/tce/code/mylex.py',117),
  ('definations -> defination definations','definations',2,'p_definations_2','/root/workspace/test_tce2/src/tce/code/mylex.py',122),
  ('defination -> struct_def ;','defination',2,'p_defination','/root/workspace/test_tce2/src/tce/code/mylex.py',127),
  ('defination -> interface_def ;','defination',2,'p_defination','/root/workspace/test_tce2/src/tce/code/mylex.py',128),
  ('defination -> sequence_def ;','defination',2,'p_defination','/root/workspace/test_tce2/src/tce/code/mylex.py',129),
  ('defination -> dictionary_def ;','defination',2,'p_defination','/root/workspace/test_tce2/src/tce/code/mylex.py',130),
  ('defination -> <empty>','defination',0,'p_defination','/root/workspace/test_tce2/src/tce/code/mylex.py',131),
  ('sequence_def -> SEQUENCE < type > IDENTIFIER','sequence_def',5,'p_sequence_def','/root/workspace/test_tce2/src/tce/code/mylex.py',138),
  ('dictionary_def -> DICTIONARY < type , type > IDENTIFIER','dictionary_def',7,'p_dictionary_def','/root/workspace/test_tce2/src/tce/code/mylex.py',163),
  ('interface_def -> INTERFACE IDENTIFIER { operatemembers }','interface_def',5,'p_interface_def','/root/workspace/test_tce2/src/tce/code/mylex.py',193),
  ('operatemembers -> operatemember','operatemembers',1,'p_operatemembers','/root/workspace/test_tce2/src/tce/code/mylex.py',220),
  ('operatemembers -> operatemember operatemembers','operatemembers',2,'p_operatemembers_2','/root/workspace/test_tce2/src/tce/code/mylex.py',232),
  ('operatemember -> callreturn IDENTIFIER ( operateparams ) ;','operatemember',6,'p_operatemember','/root/workspace/test_tce2/src/tce/code/mylex.py',242),
  ('operateparams -> type_id','operateparams',1,'p_operateparams','/root/workspace/test_tce2/src/tce/code/mylex.py',256),
  ('operateparams -> <empty>','operateparams',0,'p_operateparams','/root/workspace/test_tce2/src/tce/code/mylex.py',257),
  ('operateparams -> type_id , operateparams','operateparams',3,'p_operateparams_2','/root/workspace/test_tce2/src/tce/code/mylex.py',267),
  ('callreturn -> type','callreturn',1,'p_callreturn','/root/workspace/test_tce2/src/tce/code/mylex.py',275),
  ('struct_def -> STRUCT IDENTIFIER { datamembers }','struct_def',5,'p_struct_def','/root/workspace/test_tce2/src/tce/code/mylex.py',281),
  ('datamembers -> datamember','datamembers',1,'p_datamembers','/root/workspace/test_tce2/src/tce/code/mylex.py',305),
  ('datamembers -> datamember datamembers','datamembers',2,'p_datamembers_2','/root/workspace/test_tce2/src/tce/code/mylex.py',315),
  ('datamember -> type_id ;','datamember',2,'p_datamember','/root/workspace/test_tce2/src/tce/code/mylex.py',326),
  ('type_id -> type IDENTIFIER','type_id',2,'p_type_id','/root/workspace/test_tce2/src/tce/code/mylex.py',333),
  ('type -> IDENTIFIER','type',1,'p_type','/root/workspace/test_tce2/src/tce/code/mylex.py',347),
]
