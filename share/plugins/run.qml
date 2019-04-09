//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

import QtQuick 2.0
import MuseScore 3.0

MuseScore {
      menuPath: "Plugins.run"
      version:  "2.0"
      description: "This demo plugin runs an external command. Probably this will only work on Linux."
      requiresScore: false

      /**QProcess {
        id: proc
        }**/

      onRun: {
            console.log("run ls");
            proc.start("/bin/ls");
            var val = proc.waitForFinished(30000);
            if (val)
                  console.log(proc.readAllStandardOutput());
            Qt.quit()
            }
      }
