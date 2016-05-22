#include "elementfrequencytree.h"

ElementFrequencyTree::ElementFrequencyTree():totalDits_(0),totalDahs_(0),characterTerminations_(0),
    ditBranch_(0),dahBranch_(0),parent_(0)
{

}

ElementFrequencyTree::ElementFrequencyTree(ElementFrequencyTree *parent):
    totalDits_(0),totalDahs_(0),characterTerminations_(0),
        ditBranch_(0),dahBranch_(0),parent_(parent)
{
}

ElementFrequencyTree::~ElementFrequencyTree()
{
    if(ditBranch_){
        delete ditBranch_;
    }
    if(dahBranch_){
        delete dahBranch_;
    }
}

void ElementFrequencyTree::addDit()
{
    totalDits_++;
}

void ElementFrequencyTree::addDah()
{
    totalDahs_++;
}

void ElementFrequencyTree::addTermination()
{
    characterTerminations_++;
}

double ElementFrequencyTree::ditRatio() const
{
    int total=totalDahs_+totalDits_;
    if(total==0)return 0.6;
    else return(totalDits_/total);
}

double ElementFrequencyTree::branchProbability() const
{
    if(parent_==0)return 1;
    //Naively assume that the branch probability and dit probability are independent.
    if(this==&(parent_->ditBranch())){
        return parent_->branchProbability()*parent_->ditRatio();
    }else{
        return parent_->branchProbability()*(1-parent_->ditRatio());
    }
}

ElementFrequencyTree &ElementFrequencyTree::ditBranch()
{
    if(ditBranch_==0)ditBranch_=new ElementFrequencyTree(this);
    return *ditBranch_;
}

ElementFrequencyTree &ElementFrequencyTree::dahBranch()
{
    if(dahBranch_==0)dahBranch_=new ElementFrequencyTree(this);
    return *dahBranch_;
}

ElementFrequencyTree *ElementFrequencyTree::root() const
{
    if(parent_==0){
        //Trickery to return a non-const pointer if already at root.
        void* ptr=(void*)this;
        return (ElementFrequencyTree*) ptr;
    }else{
        return parent_->root();
    }
}

