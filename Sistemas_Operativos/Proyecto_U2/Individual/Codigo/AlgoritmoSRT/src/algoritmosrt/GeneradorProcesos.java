
package algoritmosrt;

/**
 *
 * @author FerRMZ
 *///
import java.util.Random;

public class GeneradorProcesos {
    public static Proceso generarProcesoAleatorio() {
        Random random = new Random();
        String nombre = "Proceso" + (random.nextInt(100) + 1);
        int tiempo = random.nextInt(10) + 1; 
        return new Proceso(nombre, tiempo);
    }
}

