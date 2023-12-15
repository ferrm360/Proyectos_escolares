
package algoritmosrt;

/**
 *
 * @author Ferrmz
 */
import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Random;

public class AlgoritmoSRT extends JFrame {
    private DefaultTableModel tableModel;
    private JTable processTable;
    private JButton btnAgregarProceso;
    private List<Proceso> procesos;
    private JProgressBar progressBar;

    public AlgoritmoSRT() {
        setTitle("Algoritmo SRT");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(400, 400);

        tableModel = new DefaultTableModel();
        tableModel.addColumn("Nombre del Proceso");
        tableModel.addColumn("Tiempo Restante");

        processTable = new JTable(tableModel);
        btnAgregarProceso = new JButton("Agregar Proceso Aleatorio");
        progressBar = new JProgressBar();

        add(new JScrollPane(processTable), BorderLayout.CENTER);
        add(btnAgregarProceso, BorderLayout.SOUTH);
        add(progressBar, BorderLayout.NORTH);

        btnAgregarProceso.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                agregarProcesoAleatorio();
            }
        });

        procesos = new ArrayList<>();
        mostrarProcesosEnTabla();

        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                while (true) {
                    try {
                        Thread.sleep(3000); // Esperar 3 segundos
                        SwingUtilities.invokeLater(new Runnable() {
                            @Override
                            public void run() {
                                agregarProcesoAleatorio();
                            }
                        });
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        });
        thread.start();

        Timer timer = new Timer(1000, new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                ejecutarProceso();
            }
        });
        timer.start();
    }

    private void agregarProcesoAleatorio() {
        Proceso nuevoProceso = GeneradorProcesos.generarProcesoAleatorio();
        procesos.add(nuevoProceso);

        tableModel.addRow(new Object[]{nuevoProceso.nombre, nuevoProceso.tiempoRestante});

       
        progressBar.setMaximum(nuevoProceso.tiempoTotal);
        progressBar.setValue(nuevoProceso.tiempoRestante);
        progressBar.setForeground(Color.GREEN); 
    }

    private void ejecutarProceso() {
        if (!procesos.isEmpty()) {
            Proceso procesoActual = Collections.min(procesos, (p1, p2) -> Integer.compare(p1.tiempoRestante, p2.tiempoRestante));
            procesoActual.tiempoRestante--;

            // Actualizar la barra de progreso para el proceso en ejecución
            progressBar.setValue(procesoActual.tiempoRestante);

            // Actualizar la tabla
            int rowIndex = obtenerIndiceFilaPorNombre(procesoActual.nombre);
            tableModel.setValueAt(procesoActual.tiempoRestante, rowIndex, 1);

            if (procesoActual.tiempoRestante <= 0) {
                procesos.remove(procesoActual);
                
                int rowIndexToRemove = obtenerIndiceFilaPorNombre(procesoActual.nombre);
                tableModel.removeRow(rowIndexToRemove);

               
                progressBar.setValue(0);
            }
        }
    }

    private void mostrarProcesosEnTabla() {
        tableModel.setRowCount(0);

        for (Proceso proceso : procesos) {
            tableModel.addRow(new Object[]{proceso.nombre, proceso.tiempoRestante});
        }
    }

    private int obtenerIndiceFilaPorNombre(String nombre) {
        for (int i = 0; i < tableModel.getRowCount(); i++) {
            if (tableModel.getValueAt(i, 0).equals(nombre)) {
                return i;
            }
        }
        return -1;
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                AlgoritmoSRT algoritmoSRT = new AlgoritmoSRT();
                algoritmoSRT.setVisible(true);
            }
        });
    }
}
