public class dns-server {
	public static void main(String[] args) {

		int argcount = args.length;
		int port;
		String hostfile;
		if(argcount==0) {
			port = 12345;
			hostfile = "hosts.txt";
		}
		else if(argcount==3) {
			if(args[0].equals("-p")) {
				try {
					port = Integer.parseInt(args[1]);
					hostfile = "hosts.txt";
				} catch(Exception e) {
					System.err.println("Error: Invalid port number");
					System.exit(1);
				}
			}
			else if(args[0].equals("-f")) {
				hostfile = args[1];
			}
			else {
				System.err.println("Proper cmd: dns-server [-p port#] [-f hostfile]");
				System.exit(1);
			}
		}
		else if(argcount==5) {
			if(args[0].equals("-p") && args[2].equals("-f")) {
				try {
					port = Integer.parseInt(args[1]);
					host = args[3];
				} catch(Exception e) {
					System.err.println("Error: Invalid port number");
					System.exit(1);
				}
			}
			else if(args[0].equals("-f") && args[2].equals("-p")) {
				try {
					port = Integer.parseInt(args[3]);
					host = args[1];
				} catch(Exception e) {
					System.err.println("Error: Invalid port number");
					System.exit(1);
				}
			}
			else {
				System.err.println("Proper cmd: dns-server [-p port#] [-f hostfile]");
				System.exit(1);
			}
		}
		else {
			System.err.println("Proper cmd: dns-server [-p port#] [-f hostfile]");
			System.exit(1);
		}

		Socket sock = null;
		try {
			sock = new Socket();
			sock.bind(new InetAddress(port));
		} catch(Exception e) {
			System.err.println("");
			System.exit(1);
		}









	}
}
