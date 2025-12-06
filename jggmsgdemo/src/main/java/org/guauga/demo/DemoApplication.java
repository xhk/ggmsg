package org.guauga.demo;

import jggmsg.ChannelListener;
import lombok.extern.slf4j.Slf4j;
import lombok.var;
import org.springframework.boot.CommandLineRunner;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.util.StopWatch;

@SpringBootApplication
@Slf4j
public class DemoApplication implements CommandLineRunner {

	public static void main(String[] args) {
		SpringApplication.run(DemoApplication.class, args);
	}

	@Override
	public void run(String... args) throws Exception {
		System.loadLibrary("jggmsg");

		log.trace("load ggmsg completed!");
//		if(args.length == 0){
//			return ;
//		}

		String mode = "server";
		int serviceId = 1;
		short port = 9090;

		//ChannelListener listener = new ChannelListener();

		MsgChannel mc = new MsgChannel();
		mc.init();
		if(mode == "server"){
			mc.start(serviceId, port);
			Thread.sleep(10*1000);
			// wait for client connect
			var msg = new byte[1024];
			log.info("test start");
			StopWatch sw = new StopWatch();
			sw.start();
			for(int i=0;i<100*10000;++i){
				mc.sendTestMsg(msg);
			}
			sw.stop();
			log.info("test end");
			System.out.printf("total seconds: %f \n", sw.getTotalTimeSeconds());
		}else{
			mc.connect("127.0.0.1", port);

		}

		//System.in.read();
	}
}
