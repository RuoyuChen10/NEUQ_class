public class TestEdible {
    public static void main(String[] args) {
		// TODO 自动生成的方法存根
    Object[] objects= {new Tiger(), new Chicken(), new Apple()};
    System.out.println("能吃的有：");
    for(int i=0;i<objects.length;i++)
    {
    	if(objects[i] instanceof Edible)
    		System.out.println(((Edible)objects[i]).howToEat());
    }
    System.out.println("会叫的有：");
    for(int i=0;i<objects.length;i++) {	
    if(objects[i] instanceof Animal)
    		System.out.println(((Animal)objects[i]).sound());
    }
	}

}

abstract class Animal {

	public abstract String sound(); 
}
interface Edible {

    public abstract String howToEat();

}
class Tiger extends Animal {

	@Override
	public String sound() {
		// TODO 自动生成的方法存根
		return "老虎大声吼";
	}

}
class Chicken extends Animal implements Edible {

	@Override
	public String howToEat() {
		// TODO 自动生成的方法存根
		return "鸡肉可以烤着吃";
	}

	@Override
	public String sound() {
		// TODO 自动生成的方法存根
		return "小鸡咯咯哒";
	}

}
class Apple implements Edible {

	@Override
	public String howToEat() {
		// TODO 自动生成的方法存根
		return "苹果可以榨汁！";
	}

}
