#ifndef _tce2_H
#define _tce2_H

#include <string>
#include <functional>
#include <map>
#include <vector>
#include <list>
#include <algorithm>
#include <boost/smart_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <tce/tce.h>

namespace tce2 {
	
	struct Animal{
	// -- STRUCT -- 
		int 	age;
		unsigned char 	num;
		bool 	sex;
		std::string 	name;
		//构造函数
		Animal(){
			this->age = 0;
			this->num = 0;
			this->sex = false;
			this->name = "";
		}		
		
		void marshall(tce::ByteArray& d){
			d.writeInt(this->age);
			d.writeByte(this->num);
			if(this->sex == true){
				d.writeByte(1);
			}else{
				d.writeByte(0);
			}			
			d.writeInt(this->name.size());
			d.writeString(this->name);
		}		
		
		//反序列化 unmarshall()
		bool unmarshall(tce::ByteArray& d){
			try{
				bool r = false;
				this->age = d.readInt();
				this->num = d.readByte();
				{				
					unsigned char _d = 0;
					_d = d.readByte();
					if(_d == 0){
						this->sex = false;
					}else{
						this->sex = true;
					}					
				}				
				{				
					unsigned int _d = 0;
					_d = d.readUnsignedInt();
					this->name = d.readString();
				}				
			}catch(const tce::RpcException& e){
				return false;
			}
			return true;
		}		
		 // --  end function -- 
		
	};	
	
	struct IntListhlp{
		
		typedef std::vector<int>  sequence_type;
		//# -- SEQUENCE --
		sequence_type &ds;
		
		IntListhlp(sequence_type& ds_):ds(ds){
		}		
		
		void marshall(tce::ByteArray& d){
			d.writeUnsignedInt((unsigned int)this->ds.size());
			for(size_t n=0; n< this->ds.size();n++){
				d.writeInt(this->ds[n]);
			}			
		}		
		
		bool unmarshall(tce::ByteArray& d){
			bool r = false;
			try{
				size_t _size = 0;
				_size = d.readUnsignedInt();
				for(size_t _p=0;_p < _size;_p++){
					{					
						int _o = 0;
						_o = d.readInt();
						this->ds.push_back(_o);
					}					
				}				
			}catch(const tce::RpcException& e){
				return false;
			}			
			return true;
		}		
		
	};	
	
	
	struct IdNamehlp {
		//# -- THIS IS DICTIONARY! --
		typedef std::map<int,std::string> hash_type;
		typedef std::pair<int,std::string> hash_pair;
		hash_type& ds;
		
		IdNamehlp(hash_type & ds_):ds(ds_){
		}		
		
		void marshall(tce::ByteArray& d){
			size_t _size = 0;
			_size = this->ds.size();
			d.writeUnsignedInt( this->ds.size() );
			hash_type::iterator _itr;
			for(_itr=this->ds.begin();_itr!=this->ds.end();_itr++){
				hash_pair& _pair = *_itr;
				{				
					int k = _pair.first;
					d.writeInt(k);
				}				
				{				
					std::string v = _pair.second;
					d.writeInt(v.size());
					d.writeString(v);
				}				
			}			
		}		
		
		// unmarshall()
		bool unmarshall(tce::ByteArray& d){
			bool r = false;
			try{
				size_t _size = 0;
				_size = d.readUnsignedInt();
				for(size_t _p=0;_p < _size;_p++){
					int _k = 0;
					_k = d.readInt();
					std::string _v = "";
					{					
						unsigned int _d = 0;
						_d = d.readUnsignedInt();
						_v = d.readString();
					}					
					this->ds[_k] = _v;
				}				
			
			}catch(...){
				return false;
			}			
			return true;
		}		
	};	
	//-- end Dictonary Class definations --
	
	
	struct AnimalListThlp{
		
		typedef std::vector<Animal>  sequence_type;
		//# -- SEQUENCE --
		sequence_type &ds;
		
		AnimalListThlp(sequence_type& ds_):ds(ds){
		}		
		
		void marshall(tce::ByteArray& d){
			d.writeUnsignedInt((unsigned int)this->ds.size());
			for(size_t n=0; n< this->ds.size();n++){
				this->ds[n].marshall(d);
			}			
		}		
		
		bool unmarshall(tce::ByteArray& d){
			bool r = false;
			try{
				size_t _size = 0;
				_size = d.readUnsignedInt();
				for(size_t _p=0;_p < _size;_p++){
					{					
						Animal _o;
						r= _o.unmarshall(d);
						if(!r) return false;
						this->ds.push_back(_o);
					}					
				}				
			}catch(const tce::RpcException& e){
				return false;
			}			
			return true;
		}		
		
	};	
	
	
	struct ClientInfo{
	// -- STRUCT -- 
		std::string 	id;
		int 	age;
		int 	sex;
		std::string 	addr;
		std::vector<int> 	ids;
		std::map<int,std::string> 	idnames;
		std::vector<Animal> 	animals;
		//构造函数
		ClientInfo(){
			this->id = "";
			this->age = 0;
			this->sex = 0;
			this->addr = "";
			this->ids = std::vector<int>();
			this->idnames = std::map<int,std::string>();
			this->animals = std::vector<Animal>();
		}		
		
		void marshall(tce::ByteArray& d){
			d.writeInt(this->id.size());
			d.writeString(this->id);
			d.writeInt(this->age);
			d.writeInt(this->sex);
			d.writeInt(this->addr.size());
			d.writeString(this->addr);
			{			
				IntListhlp _c(this->ids);
				_c.marshall(d);
			}			
			{			
				IdNamehlp _c(this->idnames);
				_c.marshall(d);
			}			
			{			
				AnimalListThlp _c(this->animals);
				_c.marshall(d);
			}			
		}		
		
		//反序列化 unmarshall()
		bool unmarshall(tce::ByteArray& d){
			try{
				bool r = false;
				{				
					unsigned int _d = 0;
					_d = d.readUnsignedInt();
					this->id = d.readString();
				}				
				this->age = d.readInt();
				this->sex = d.readInt();
				{				
					unsigned int _d = 0;
					_d = d.readUnsignedInt();
					this->addr = d.readString();
				}				
				{				
					IntListhlp _c(this->ids);
					r = _c.unmarshall(d);
					if(!r){return false;}
				}				
				{				
					IdNamehlp _c(this->idnames);
					r = _c.unmarshall(d);
					if(!r){return false;}
				}				
				{				
					AnimalListThlp _c(this->animals);
					r = _c.unmarshall(d);
					if(!r){return false;}
				}				
			}catch(const tce::RpcException& e){
				return false;
			}
			return true;
		}		
		 // --  end function -- 
		
	};	
	class PostClient_delegate;
	
	class PostClient:public tce::RpcServant{
		//# -- INTERFACE -- 
		public:
		
		boost::shared_ptr<PostClient_delegate> delegate;
		PostClient::PostClient();
		
		
		ClientInfo getInfo(tce::RpcContext& ctx){
			return ClientInfo();
		}		
		
		int test1(const std::string& name,const std::vector<int>& ids,tce::RpcContext& ctx){
			return 0;
		}		
		
		std::map<int,std::string> test2(const std::string& name,tce::RpcContext& ctx){
			return std::map<int,std::string>();
		}		
	};	
	
	class PostClient_delegate:public tce::RpcServantDelegate {
		
		public:
		PostClient * inst;
		
		PostClient_delegate(PostClient* inst){
			this->_index = 0;
			this->_optlist[0] = &PostClient_delegate::getInfo_dummy;
			this->_optlist[1] = &PostClient_delegate::test1_dummy;
			this->_optlist[2] = &PostClient_delegate::test2_dummy;
			this->inst = inst;
		}		
		
		static bool getInfo_dummy(tce::RpcContext& ctx){
			PostClient_delegate *pf = (PostClient_delegate*) ctx.delegate;
			return pf->getInfo(ctx);
		}		
		
		bool getInfo(tce::RpcContext& ctx){
			tce::ByteArray& d = ctx.msg->paramstream;
			bool r = false;
			ClientInfo cr;
			cr = this->inst->getInfo(ctx);
			if( ctx.msg->calltype & tce::RpcMessage::ONEWAY){
				return false;
			}			
			
			boost::shared_ptr<tce::ByteArray> _d(new tce::ByteArray);
			tce::ByteArray& d = *_d;
			boost::shared_ptr<tce::RpcMessageReturn> m(new tce::RpcMessageReturn);
			m->sequence = ctx.msg->sequence;
			cr.marshall(d);
			if(d.size()) m->addParam(_d);
			ctx.conn->sendMessage(m);
			return true;
		}		
		
		static bool test1_dummy(tce::RpcContext& ctx){
			PostClient_delegate *pf = (PostClient_delegate*) ctx.delegate;
			return pf->test1(ctx);
		}		
		
		bool test1(tce::RpcContext& ctx){
			tce::ByteArray& d = ctx.msg->paramstream;
			bool r = false;
			std::string name;
			{			
				unsigned int _d = 0;
				_d = d.readUnsignedInt();
				name = d.readString();
			}			
			std::vector<int> ids;
			{			
				IntListhlp _c(ids);
				r = _c.unmarshall(d);
				if(!r) return false;
			}			
			int cr;
			cr = this->inst->test1(name,ids,ctx);
			if( ctx.msg->calltype & tce::RpcMessage::ONEWAY){
				return false;
			}			
			
			boost::shared_ptr<tce::ByteArray> _d(new tce::ByteArray);
			tce::ByteArray& d = *_d;
			boost::shared_ptr<tce::RpcMessageReturn> m(new tce::RpcMessageReturn);
			m->sequence = ctx.msg->sequence;
			d.writeInt(cr);
			if(d.size()) m->addParam(_d);
			ctx.conn->sendMessage(m);
			return true;
		}		
		
		static bool test2_dummy(tce::RpcContext& ctx){
			PostClient_delegate *pf = (PostClient_delegate*) ctx.delegate;
			return pf->test2(ctx);
		}		
		
		bool test2(tce::RpcContext& ctx){
			tce::ByteArray& d = ctx.msg->paramstream;
			bool r = false;
			std::string name;
			{			
				unsigned int _d = 0;
				_d = d.readUnsignedInt();
				name = d.readString();
			}			
			std::map<int,std::string> cr;
			cr = this->inst->test2(name,ctx);
			if( ctx.msg->calltype & tce::RpcMessage::ONEWAY){
				return false;
			}			
			
			boost::shared_ptr<tce::ByteArray> _d(new tce::ByteArray);
			tce::ByteArray& d = *_d;
			boost::shared_ptr<tce::RpcMessageReturn> m(new tce::RpcMessageReturn);
			m->sequence = ctx.msg->sequence;
			{			
				IdNamehlp _c_1(cr);
				_c_1.marshall(d);
			}			
			if(d.size()) m->addParam(_d);
			ctx.conn->sendMessage(m);
			return true;
		}		
		
	};	
	class PostClient_AsyncCallBack: tce::RpcAsyncCallBackBase{
	protected:
		virtual void getInfo(const ClientInfo & result,PostClientPrx* prx){
		}		
		
		virtual void test1(const int & result,PostClientPrx* prx){
		}		
		
		virtual void test2(const std::map<int,std::string> & result,PostClientPrx* prx){
		}		
		
	};	
	
	
	typedef boost::shared_ptr<PostClient> PostClientPrxPtr;
	class PostClientPrx: public tce::RpcProxyBase{
		//# -- INTERFACE PROXY -- 
		
		PostClientPrx( boost::shared_ptr<tce::RpcConnection> & conn){
			this->conn = conn;
		}		
		~PostClientPrx(){}
		public:
		
		
		
		ClientInfo getInfo(unsigned int timeout = 0) throw(tce::RpcException){
			//# function index: 5
			
			boost::shared_ptr<tce::RpcMessageCall> m(new tce::RpcMessageCall(tce::RpcMessage::TWOWAY));
			m->ifidx = 0;
			m->opidx = 0;
			m->timeout = timeout;
			m->issuetime = tce::RpcCommunicator::instance().currentTimeStamp(); 
			boost::shared_ptr<tce::ByteArray> _d(new tce::ByteArray);
			tce::ByteArray& d = *_d;
			m->prx =(RpcProxyBase*)this;
			m->conn = this->conn;
			this->conn->sendMessage(m);
			m->wait = boost::shared_ptr< tce::MutexObject< tce::RpcMessage> >( new tce::MutexObject< tce::RpcMessage> );
			boost::shared_ptr<tce::RpcMessage> mr = m->wait->waitObject(timeout);
			if(!mr.get()){
				throw tce::RpcException(tce::RpcConsts::RPCERROR_TIMEOUT);
			}			
			if(mr->exception.get()){
				throw *mr->exception.get();
			}			
			tce::ByteArray& d = mr->paramstream;
			ClientInfo p;
			p.unmarshall(d);
			return p;
		}		
		
		void getInfo(PostClient_AsyncCallBack* async) throw(tce::RpcException){
			//# function index: 5
			
			boost::shared_ptr<tce::RpcMessageCall> m(new tce::RpcMessageCall(tce::RpcMessage::ASYNC));
			m->ifidx = 0;
			m->opidx = 0;
			m->issuetime = tce::RpcCommunicator::instance().currentTimeStamp(); 
			boost::shared_ptr<tce::ByteArray> _d(new tce::ByteArray);
			tce::ByteArray& d = *_d;
			m->prx =(tce::RpcProxyBase*)this;
			m->async = async;
			m->callback = PostClientPrx::getInfo_asyncparser_hlp;
			m->conn = this->conn;
			this->conn->sendMessage(m);
		}		
		
		
		static void getInfo_asyncparser_hlp(tce::RpcMessage* m,tce::RpcMessage* m2){
			PostClientPrx* pthis = (PostClientPrx*)m->prx;
			pthis->getInfo_asyncparser(m,m2);
		}		
		
		
		void getInfo_asyncparser(tce::RpcMessage* m_,tce::RpcMessage* m2_){
			//# function index: 5 , m2 - callreturn msg.
			
			tce::RpcMessage& m = *m_;
			tce::RpcMessage& m2 = *m2_;
			tce::ByteArray& stream = m2.paramstream;
			PostClient_AsyncCallBack* user = (PostClient_AsyncCallBack*)m.async;
			if(stream.size() == 0){ 
				return;
			}			
			try{
				tce::ByteArray d;
				d = stream;
				ClientInfo _p;
				_p.unmarshall(d);
				user->getInfo(_p,this);
			}catch(const tce::RpcException& e){
				tce::RpcCommunicator::instance().logTrace(e.what());
			}			
		}		
		
		
		
		int test1(std::string name,std::vector<int> ids,unsigned int timeout = 0) throw(tce::RpcException){
			//# function index: 5
			
			boost::shared_ptr<tce::RpcMessageCall> m(new tce::RpcMessageCall(tce::RpcMessage::TWOWAY));
			m->ifidx = 0;
			m->opidx = 1;
			m->timeout = timeout;
			m->issuetime = tce::RpcCommunicator::instance().currentTimeStamp(); 
			boost::shared_ptr<tce::ByteArray> _d(new tce::ByteArray);
			tce::ByteArray& d = *_d;
			d.writeInt(name.size());
			d.writeString(name);
			m->addParam(_d);
			IntListhlp _c_2(ids);
			_c_2.marshall(d);
			m->addParam(_d);
			m->prx =(RpcProxyBase*)this;
			m->conn = this->conn;
			this->conn->sendMessage(m);
			m->wait = boost::shared_ptr< tce::MutexObject< tce::RpcMessage> >( new tce::MutexObject< tce::RpcMessage> );
			boost::shared_ptr<tce::RpcMessage> mr = m->wait->waitObject(timeout);
			if(!mr.get()){
				throw tce::RpcException(tce::RpcConsts::RPCERROR_TIMEOUT);
			}			
			if(mr->exception.get()){
				throw *mr->exception.get();
			}			
			tce::ByteArray& d = mr->paramstream;
			int p;
			p = d.readInt();
			return p;
		}		
		
		void test1(std::string name,std::vector<int> ids,PostClient_AsyncCallBack* async) throw(tce::RpcException){
			//# function index: 5
			
			boost::shared_ptr<tce::RpcMessageCall> m(new tce::RpcMessageCall(tce::RpcMessage::ASYNC));
			m->ifidx = 0;
			m->opidx = 1;
			m->issuetime = tce::RpcCommunicator::instance().currentTimeStamp(); 
			boost::shared_ptr<tce::ByteArray> _d(new tce::ByteArray);
			tce::ByteArray& d = *_d;
			d.writeInt(name.size());
			d.writeString(name);
			m->addParam(_d);
			IntListhlp _c_3(ids);
			_c_3.marshall(d);
			m->addParam(_d);
			m->prx =(tce::RpcProxyBase*)this;
			m->async = async;
			m->callback = PostClientPrx::test1_asyncparser_hlp;
			m->conn = this->conn;
			this->conn->sendMessage(m);
		}		
		
		
		static void test1_asyncparser_hlp(tce::RpcMessage* m,tce::RpcMessage* m2){
			PostClientPrx* pthis = (PostClientPrx*)m->prx;
			pthis->test1_asyncparser(m,m2);
		}		
		
		
		void test1_asyncparser(tce::RpcMessage* m_,tce::RpcMessage* m2_){
			//# function index: 5 , m2 - callreturn msg.
			
			tce::RpcMessage& m = *m_;
			tce::RpcMessage& m2 = *m2_;
			tce::ByteArray& stream = m2.paramstream;
			PostClient_AsyncCallBack* user = (PostClient_AsyncCallBack*)m.async;
			if(stream.size() == 0){ 
				return;
			}			
			try{
				tce::ByteArray d;
				d = stream;
				int _p = 0;
				_p = d.readInt();
				user->test1(_p,this);
			}catch(const tce::RpcException& e){
				tce::RpcCommunicator::instance().logTrace(e.what());
			}			
		}		
		
		
		
		std::map<int,std::string> test2(std::string name,unsigned int timeout = 0) throw(tce::RpcException){
			//# function index: 5
			
			boost::shared_ptr<tce::RpcMessageCall> m(new tce::RpcMessageCall(tce::RpcMessage::TWOWAY));
			m->ifidx = 0;
			m->opidx = 2;
			m->timeout = timeout;
			m->issuetime = tce::RpcCommunicator::instance().currentTimeStamp(); 
			boost::shared_ptr<tce::ByteArray> _d(new tce::ByteArray);
			tce::ByteArray& d = *_d;
			d.writeInt(name.size());
			d.writeString(name);
			m->addParam(_d);
			m->prx =(RpcProxyBase*)this;
			m->conn = this->conn;
			this->conn->sendMessage(m);
			m->wait = boost::shared_ptr< tce::MutexObject< tce::RpcMessage> >( new tce::MutexObject< tce::RpcMessage> );
			boost::shared_ptr<tce::RpcMessage> mr = m->wait->waitObject(timeout);
			if(!mr.get()){
				throw tce::RpcException(tce::RpcConsts::RPCERROR_TIMEOUT);
			}			
			if(mr->exception.get()){
				throw *mr->exception.get();
			}			
			tce::ByteArray& d = mr->paramstream;
			std::map<int,std::string> p;
			IdNamehlp _c(p);
			_c.unmarshall(d);
			return p;
		}		
		
		void test2(std::string name,PostClient_AsyncCallBack* async) throw(tce::RpcException){
			//# function index: 5
			
			boost::shared_ptr<tce::RpcMessageCall> m(new tce::RpcMessageCall(tce::RpcMessage::ASYNC));
			m->ifidx = 0;
			m->opidx = 2;
			m->issuetime = tce::RpcCommunicator::instance().currentTimeStamp(); 
			boost::shared_ptr<tce::ByteArray> _d(new tce::ByteArray);
			tce::ByteArray& d = *_d;
			d.writeInt(name.size());
			d.writeString(name);
			m->addParam(_d);
			m->prx =(tce::RpcProxyBase*)this;
			m->async = async;
			m->callback = PostClientPrx::test2_asyncparser_hlp;
			m->conn = this->conn;
			this->conn->sendMessage(m);
		}		
		
		
		static void test2_asyncparser_hlp(tce::RpcMessage* m,tce::RpcMessage* m2){
			PostClientPrx* pthis = (PostClientPrx*)m->prx;
			pthis->test2_asyncparser(m,m2);
		}		
		
		
		void test2_asyncparser(tce::RpcMessage* m_,tce::RpcMessage* m2_){
			//# function index: 5 , m2 - callreturn msg.
			
			tce::RpcMessage& m = *m_;
			tce::RpcMessage& m2 = *m2_;
			tce::ByteArray& stream = m2.paramstream;
			PostClient_AsyncCallBack* user = (PostClient_AsyncCallBack*)m.async;
			if(stream.size() == 0){ 
				return;
			}			
			try{
				tce::ByteArray d;
				d = stream;
				std::map<int,std::string> _p;
				IdNamehlp _c(_p);
				_c.marshall(d);
				user->test2(_p,this);
			}catch(const tce::RpcException& e){
				tce::RpcCommunicator::instance().logTrace(e.what());
			}			
		}		
		
		static PostClientPrxPtr create(boost::shared_ptr<tce::RpcConnection>& conn){
			return PostClientPrxPtr( new PostClientPrx(conn) );
		}		
	};	
	
	inline PostClient::PostClient(){
		this->_delegate = boost::shared_ptr<PostClient_delegate>( new PostClient_delegate(this) );
	}	
	
}
#endif
