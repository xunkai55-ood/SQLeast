#include <sqleast.h>
#include "ix/indexscan.h"

namespace sqleast{
    namespace ix{
        IndexScan::IndexScan(Index& i, int v, CompOp c) :index_(i), value_(v), compOp_(c)
        {
            canDo = true;
            switch(compOp_){
                case NO_OP:
                    toStart1();
                    break;
                case EQ_OP:
                    toStart1();
                    break;
                case LT_OP:
                    break;
                case GT_OP:
                    break;
                case LE_OP:
                    break;
                case GE_OP:
                    break;
                case NE_OP:
                    toStart1();
                    break;
                default:
                    break;
            }
        }

        IndexScan::~IndexScan() {
        }

        RID IndexScan::next() {
            RID res(-1,-1);
            if(!canDo)
                return res;
            switch(compOp_){
                case NO_OP:
                    res = NoSearch();
                    break;
                /*case EQ_OP:
                    res = EqSearch();
                    break;
                case LT_OP:
                    res = LtSearch();
                    break;
                case GT_OP:
                    res = GtSearch();
                    break;
                case LE_OP:
                    res = LeSearch();
                    break;
                case GE_OP:
                    res = GeSearch();
                    break;
                case NE_OP:
                    res = NeSearch();
                    break;*/
                default:
                    break;
            }
            return res;
        }

        void IndexScan::toStart1() {
            current = index_.getRootRID();
            Node root;
            index_.getNode(current, root);
            if(root.size == 0)  return;
            while(!root.isLeaf){
                current = root.n[0];
                index_.getNode(current, root);
                position.push_back(0);
            }
            position.push_back(0);
        }

        bool IndexScan::toRight(){
            int num = position.back();
            Node n;
            index_.getNode(current, n);
            if(num < n.size - 1){
                num ++;
                position[position.size()-1] = num;
                return true;
            }

            if(current == index_.getRootRID()) return false;
            current = n.parent;
            index_.getNode(current, n);
            position.pop_back();
            num = position.back();
            while(num == n.size){
                if(current == index_.getRootRID()) return false;
                current = n.parent;
                index_.getNode(current, n);
                position.pop_back();
                num = position.back();
            }
            if(num < n.size){
                num ++;
                position[position.size()-1] = num;
            }
            while(!n.isLeaf){
                num = position.back();
                current = n.n[num];
                index_.getNode(current, n);
                position.push_back(0);
            }
            return true;
        }

        RID IndexScan::NoSearch(){
            if(position.size() == 0) return RID(-1,-1);
            int num = position.back();
            Node n;
            index_.getNode(current, n);
            RID res = n.n[num];
            canDo = toRight();
            return res;
        }

        RID IndexScan::EqSearch(){
            if(position.size() == 0) return RID(-1,-1);
            int num = position.back();
            Node n;
            index_.getNode(current, n);
            RID res = n.n[num];
            while(n.k[num] != value_){
                if(canDo) {
                    toRight();
                    num = position.back();
                    index_.getNode(current, n);
                    res = n.n[num];
                }
                else
                    return RID(-1,-1);
            }
            return res;
        }
    }
}