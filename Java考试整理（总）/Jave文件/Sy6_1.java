public class Sy6_1 {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		Circle c = new Circle(2,10);
		Rectangle R = new Rectangle(2,3,10);
		Triangle t = new Triangle(7,8);
		c.print();
		R.print();
		t.print();
	}

}

abstract class Virticle{
	public abstract double virticle();
}

class Circle extends Virticle{
	double r,h;
	Circle(double _r_,double _h_)
	{
		this.r=_r_;
		this.h=_h_;
	}
	public double virticle() {
		return 3.1415*r*r*h;
	}
	void print()
	{
		System.out.println("圆柱体的体积是："+this.virticle());
	}
}

class Rectangle extends Virticle{	//矩形
	double x,y,h;
	Rectangle(double _x_,double _y_,double _h_)
	{
		this.x=_x_;
		this.y=_y_;
		this.h=_h_;
	}
	public double virticle() {
		return x*y*h;
	}
	void print()
	{
		System.out.println("矩形柱体的体积是："+this.virticle());
	}
}

class Triangle extends Virticle{	
	double t,h;
	Triangle(double _t_,double _h_)
	{
		this.t=_t_;
		this.h=_h_;
	}
	public double virticle() {
		return t*t*1.73/2*h;
	}
	void print()
	{
		System.out.println("底为等边三角形柱体的体积是："+this.virticle());
	}
}