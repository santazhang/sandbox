package dbrocks.ast;

import java.util.ArrayList;

import dbrocks.util.BiName;

public class Select extends Statement {
	
	Expr criterion;
	
	ArrayList<BiName> selectItems;
	
	boolean selectAll = false;
	
	public void setCriterion(Expr criterion) {
		this.criterion = criterion;
	}
	
	public Expr getCriterion() {
		return criterion;
	}
	
	public void setSelectAll(boolean selectAll) {
		this.selectAll = selectAll;
	}
	
	public boolean isSelectAll() {
		return selectAll;
	}
	
	public void setSelectItems(ArrayList<BiName> selectItems) {
		this.selectItems = selectItems; 
	}

	@Override
	public void accept(Visitor visitor) {
		visitor.visit(this);
	}

}
