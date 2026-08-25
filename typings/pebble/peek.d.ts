/*
* Copyright (c) 2026 Pebble contributors
*
*   This file is part of the Moddable SDK Tools.
*
*   The Moddable SDK Tools is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   The Moddable SDK Tools is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with the Moddable SDK Tools.  If not, see <http://www.gnu.org/licenses/>.
*
*/

type PeekWidgetResult = "success" | "invalidArgs" | "disabled";

interface PeekWidgetOptions {
  /**
   * Icon shown in the widget's icon box: a publishedMedia id. Omit it to use the app's
   * own icon.
   */
  icon?: number;
  /** The widget's title. Required. Truncated at 32 bytes of UTF-8. */
  title: string;
  /** The widget's subtitle, shown below the title. Truncated at 32 bytes of UTF-8. */
  subtitle?: string;
  /**
   * Handed to the app through watch.launch.arguments when the widget's button shortcut
   * launches it.
   */
  launchCode?: number;
  /** Seconds until the widget withdraws itself; 0 or absent to stay until withdrawn. */
  timeout?: number;
}

/**
 * Publishes contextual widgets to the Quick View surface at the bottom of the
 * user's watchface.
 */
declare class PeekWidget {
  /** Publishes (or replaces) this app's Quick View widget. */
  static publish(options: PeekWidgetOptions): PeekWidgetResult;
  /** Withdraws this app's Quick View widget, if it has one. */
  static withdraw(): void;
}

export default PeekWidget;
