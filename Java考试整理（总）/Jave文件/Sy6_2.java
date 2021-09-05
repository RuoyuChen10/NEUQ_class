public class Sy6_2 {
	public static void main(String[] args) {
		// TODO 自动生成的方法存根
    Object[] objects= {new Tiger(), new Chicken(), new Apple(), new Cat(), new Fish()};
    System.out.println("能吃的有：");
    for(int i=0;i<objects.length;i++)
    {
    	if(objects[i] instanceof Edible)
    		System.out.println(((Edible)objects[i]).howToEat());
    }
    System.out.println("\n会叫的有：");
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

class Tiger extends Animal{
	public String sound() {
		return "老虎大声吼";
	}
}

class Chicken extends Animal implements Edible{
	public String sound() {
		return "小鸡叽叽喳喳叽叽喳喳的叫";
	}

	@Override
	public String howToEat() {
		return "小鸡可以炖着吃，可以煮着吃，也可以烤着吃";
	}
}

class Apple implements Edible{

	@Override
	public String howToEat() {
		return "苹果可以直接啃着吃，也可以榨成果汁喝";
	}
	
}

class Cat extends Animal {
	public String sound(){
		return "小猫喵喵的叫";
	}
}

class Fish implements Edible{

	@Override
	public String howToEat() {
		// TODO Auto-generated method stub
		return "鱼肉煮着吃，红烧着吃，烤着吃";
	}
	
}