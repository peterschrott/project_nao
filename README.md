project_nao
===========

Repository for the Automotiv Project: Steuerung eines humanoiden Roboters mittels Handzeichen

  * 'README.md': ein kurzer Überblick des Repository Inhaltes
  * 'Sandbox': eine Datei zum rumspielen, mit der erste Erfahrungen mit einem Versionsverwaltungs-Tool gesammelt werden können
  
./experimental:

  * 'BlobDetetion/': der Ursprung der Blob-Erkennung. Ein C-Programm das eine öffentlich zugängliche Bibliothek zu Blob-Erkennung in Beispielvideos verwendet. Dieses Programm ist unabhängig vom NAO auf jedem Entwicklungsrechner ausführbar.
  * 'BlobDetectionNao': Hier entstand ursprünglich das lokale Modul in das die Quellen aus dem C-Programm für die Blob-Erkennung eingeflossen sind. Diese Quellen sollten nicht mehr genutzt werden, da das Modul jetzt in den Ordner 'project_nao/src/' überführt wurde. Aus Gründen der Nachvollziehbarkeit verschiedener Entwicklungsschritte bleibt dieses Repository erhalten. 
  * 'ThreadTest/': Dies ist ein lokales Modul zur Ausführung auf dem Roboter, welches demonstriert, dass es möglich ist, parallel Abläufe mittels Threads zu realisieren.
  * 'capturevideo/': Ein lokales Modul, entwickelt um Beispielvideos aufzunehmen.
  * 'choregraphe/': Hier finden sich Behaviours, die mit dem Tool Choregraphe entwickelt wurden. Diese werden hier aus Backup-Gründen hinterlegt.
  * 'cvblob/': Die Bibliothek zur Blob-Erkennung. Diese Quelldateien wurden in den aktuellen Softwarestand übernommen.
  * 'debug_test/': Dieser Ordner beinhaltet ein kleines C-Programm welches verwendet wurde um den gdb-Debugger auf dem NAO zu testen und analysieren. Ebenfalls ist eine Liste mit nützlichen Kommandos des gdb-Debuggers zu finden.
  * 'math/': Entwicklungsstand der Quelldateien zur Berechnung des Neigungswinkels des Roboterkopfes um eine ein Objekt zu fokussieren. Diese Quelldateien wurden in den aktuellen Softwarestand übernommen.
  * 'twomodules/': beinhaltet eine Demonstrations-Programm zur  parallelen Ausführung zweier lokaler Module.
  * 'autoload.ini': Eine Kopie der Autostart-Datei, die naoQi, das Betriebssystem des NAO, verwendet um lokale Module beim starten des Roboters zu laden.

./ src:

  * Alle Projektspezifischen Quelldateien 
  * 'math\' beinhaltet Quelldateien zur Berechnung des Neigungswinkels des Roboterkopfes um eine ein Objekt zu fokussieren.
  * 'cvblob\' beinhaltet alle zur Bloberkennung relevanten Quelldateien