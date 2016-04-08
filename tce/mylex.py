#--coding:utf-8--


#scott  shanghai china
#86-13916624477 qq:24509826 msn: socketref@hotmail.com
#

import	sys



if	".."	not	in	sys.path:	sys.path.insert(0,"..")

import os,sys,os.path,struct,time,traceback,string

import	ply.lex	as	lex
import ply.yacc as yacc

from lexparser import *
import lexparser as myparser

'''
默认是SLR，我们也可以通过参数指定为 LALR(

idl
interface定义 函数参数保留名称： d,idx,m
'''


tokens=(
		'IDENTIFIER','STRUCT','NUMBER','INTERFACE',
		'SEQUENCE','DICTIONARY','EXCEPTION','COMMENTLINE',
#		'VOID',

	)

def t_COMMENTLINE(t):
	'//.*\n'

def t_SEQUENCE(t):
	'sequence'
	return t

def t_DICTIONARY(t):
	'dictionary'
	return t


def t_STRUCT(t):
	'struct'
	return t


def t_INTERFACE(t):
	'interface'
	return t

#def t_VOID(t):
#	'void'
#	return t

def t_IDENTIFIER(t):
	'[A-Za-z_][A-Za-z0-9_]*'
	return t

t_NUMBER=r'\d+([uU]|[lL]|[uU][lL]|[lL][uU])?'

t_ignore	=" 	\t"

def	t_newline(t):
	r'\n+'
	t.lexer.lineno	+=	1 #int(t.value) #.count("\n")


def	t_error(t):
	print("Illegal	character	'%s'"	%	t.value[0])
	t.lexer.skip(1)

literals = [ '{','}',';','(',')','<','>',',' ]

#	Build	the	lexer
lexer = lex.lex()

#literals = [ ':',',','(',')' ]



#lexer.input(data)

#while 1:
#	tok = lexer.token()
#	if not tok: break	
	#print tok.type,tok.value
#sys.exit()


def p_start(t):
	'''start : definations
	'''

	defs = t[1]
	unit = Unit()

	defs.reverse()

#	print defs
	for d in defs:
		unit.addChild( d )


	t[0] = unit

	#t[0] = t[1]
	#print t[0]

def p_definations(t):
	'''definations : defination '''

	t[0] = [t[1],]

def p_definations_2(t):
	'''definations : defination  definations  '''
	t[2].append(t[1])
	t[0]= t[2]

def p_defination(t):
	''' defination : struct_def ';'
			| interface_def ';'
			| sequence_def ';'
			| dictionary_def ';'
			|
	'''
	t[0]=t[1]


def p_sequence_def(t):
	'''
		sequence_def : SEQUENCE '<' type '>' IDENTIFIER
	'''
#	print 'sequence..',t[3]
	name = t[5]

	if not checkVariantName(name,False):
		print 'error: sequence<%s>.%s illegal!'%(t[3],name)
		sys.exit()

	if getTypeDef(name):
		print 'error: line %s sequence (%s) has existed!'%(t.lineno(1),name)
		sys.exit() #sequence的类型名存在

	type_ = t[3]
#	if not getTypeDef(type_):
#		print 'error: sequence %s <%s> not defined!'%(name,type_)
#		sys.exit()
	print 'xxx.',name,type_

	seq = Sequence(name,type_)
	types_def[name] = seq
	t[0] = seq

def p_dictionary_def(t):
	'''
		dictionary_def : DICTIONARY '<' type ',' type '>' IDENTIFIER
	'''
	first = t[3]
	second = t[5]
	name = t[7]
	print first,second,name

	if not checkVariantName(name,False):
		print 'error: dictionary < %s > illegal!'%(name)
		sys.exit()

	if getTypeDef(name):
		print 'error: line %s dictionary type (%s) has existed!'%(t.lineno(1),name)
		sys.exit() #sequence的类型名存在


#	if not getTypeDef(first):
#		print 'error: dictionary %s.first <%s> not defined!'%(name,first)
#		sys.exit()
#
#	if not getTypeDef(second):
#		print 'error: dictionary %s.second <%s> not defined!'%(name,second)
#		sys.exit()

	dict = Dictionary(name,first,second)
	types_def[name] = dict
	t[0] = dict

def p_interface_def(t):
	'''
		interface_def :  INTERFACE IDENTIFIER '{' operatemembers '}'
	'''
#	print t[1],t[2]
	id = t[2]
	type_ = getTypeDef(id)
	if type_:
		print 'error: interface name(%s) has existed!'%id
		sys.exit()

	ifc = Interface(id)
	opms = t[4]
	opms.reverse()
	#检测函数名称是否有重复
#	print opms
	for opm in opms:
		if not ifc.createOperateMember(opm):

			print 'error: line %s createOperateMember failed! interface:: %s.%s'%( t.lineno(3),t[2],opm.name)
			sys.exit()
#		print type(ifc.children)
	types_def[id] = ifc
	t[0] = ifc # reduce to syntax tree  ...



def p_operatemembers(t):
	'''
		operatemembers : operatemember
	'''
	#print 'operatemember num:',len(t) # 1 means no operatemebmer
#	if len(t) > 1:
#		t[0] = t[1]
#	else:
#		t[0] = []
	t[0] = [t[1],]


def p_operatemembers_2(t):
	'''
		operatemembers : operatemember operatemembers
	'''
	if type(t[2]) != type([]):
		t[0] = [t[1],t[2]]
	else:
		t[2].append(t[1])
		t[0] = t[2]

def p_operatemember(t):
	'''
		operatemember : callreturn IDENTIFIER '(' operateparams ')' ';'

	'''
	params = t[4]
	params.reverse()	# 这里必须进行倒置一下
	for p in params:
		if p.id in ('d','p','m','r','idx','size','cr','container','o','prx','m2'):
			p.id+='_'
	opm = OperateMember(t[2],t[1],params)
	t[0] = opm
#	print 'x1.',opm

def p_operateparams(t):
	'''
		operateparams : type_id
			|
	'''
	if len(t) > 1:
		t[0] = [ t[1],]
	else:
		t[0] =[ ]


def p_operateparams_2(t):
	'''
		operateparams : type_id ',' operateparams
	'''

	t[3].append(t[1])
	t[0] = t[3]

def p_callreturn(t):
	'''
		callreturn : type
	'''
	t[0] = t[1]


def p_struct_def(t):
	''' struct_def : STRUCT IDENTIFIER '{' datamembers '}' '''
	id = t[2]

	type = getTypeDef(id) 
	if  type:
		print 'error struct name:%s existed!'%(id)
		sys.exit()

	st  = Struct(id)
#	print t[4]
	t[4].reverse()
	for dm in t[4]:
		if not st.createDataMember(dm):
			print 'error: datamember<%s> name<%s> has existed!'%(id,dm.id)
			sys.exit()

	types_def[id] = st	#注册数据类型到全局类型表
	#print types_def
	t[0] = st



def p_datamembers(t):
	'''
	datamembers :  datamember 
	'''
	#print t[1]
	#t[0] = t[1]
	t[0] = [t[1],]
	#print 'a1..'
	#print t[0]

def p_datamembers_2(t):
	'''
	datamembers :     datamember datamembers
	'''
	if type(t[2]) != type([]):
		t[0] = [t[1],t[2]]
	else:
		t[2].append(t[1])
		t[0] = t[2]


def p_datamember(t):
	'''
	datamember : type_id ';'
	'''
	#print 'datamenber..'
	t[0] =  t[1]

def p_type_id(t):
	'''
		type_id : type IDENTIFIER
	'''
	#print t[1],t[2]
	id = t[2]
	if not checkVariantName(id):
		print 'error: type_id.id<%s> illegal!'%(id)
		sys.exit()

	t[0] = TypeId(t[1],t[2])




def p_type(t):
	'''type : IDENTIFIER
	'''
	type = getTypeDef(t[1])

	if type:
		t[0] = type
	else:
		print 'error: line %s'%(t.lineno),' type:%s not existed!'%t[1]
		sys.exit()


def p_error(t):
	print("Syntax error at '%s'" % t)
	#print t.lineno,t.lexpos

yacc.yacc(debug=True,method="SLR")

data='''

sequence < int > IntList;

struct student { 
	int x;
	int y;
	int z;
	double fractor;
	IntList ids;
};

interface booksystem{
	int test1(int age);
	int test2(int age);
};

struct dog{
	int name;
	string dog;
	int name2;
	student st1;
};



'''

data='''

struct animal{
	int x;
};

sequence < animal > IntList;

struct student {
	int x;
	int y;
	int z;
	double fractor;
	IntList ids;
};

interface booksystem{
	int test1(int age);
	int test2(int age);
};

sequence<IntList>  XX;

dictionary<int,int> int2_t;

dictionary<int2_t,string> intstr_t;


'''

def syntax_result(data):
	return yacc.parse(data)





