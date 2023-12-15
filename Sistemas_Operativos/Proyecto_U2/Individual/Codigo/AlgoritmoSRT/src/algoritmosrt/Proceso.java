
package algoritmosrt;

/**
 *
 * @author FerRMZ
 */

public class Proceso {
    String nombre;
    int tiempoTotal;
    int tiempoRestante;

    public Proceso(String nombre, int tiempoTotal) {
        this.nombre = nombre;
        this.tiempoTotal = tiempoTotal;
        this.tiempoRestante = tiempoTotal;
    }
}

