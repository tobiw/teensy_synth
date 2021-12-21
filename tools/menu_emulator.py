import curses


curses.initscr()
curses.start_color()
curses.noecho()
curses.cbreak()
curses.curs_set(0)
curses.init_pair(1, curses.COLOR_WHITE, curses.COLOR_BLACK)
staticwin = curses.newwin(8, 32, 1, 1)
staticwin.box()


class Menu:
    def __init__(self, pages):
        self.pages = pages
        self.current_page_index = 0
        self.current_page = pages[self.current_page_index]

    def draw(self):
        self.current_page.draw()

    def rotate_left(self):
        if self.current_page.current_selection == 0:
            self.current_page_index = max(0, self.current_page_index - 1)
            self.current_page = pages[self.current_page_index]
            self.current_page.reset()
        else:
            self.current_page.decrease_value()

    def rotate_right(self):
        if self.current_page.current_selection == 0:
            self.current_page_index = min(len(self.pages) - 1, self.current_page_index + 1)
            self.current_page = pages[self.current_page_index]
            self.current_page.reset()
        else:
            self.current_page.increase_value()

    def select(self):
        self.current_page.select()


class PageParams:
    def __init__(self, param_names, params, limits, repr_map=None):
        # Need to copy lists and dicts as passed in arguments might be references to the same list/dict (e.g. multiple oscillators)
        self._param_names = param_names[:]
        self._params = params[:]  # must be list of parameter names
        self._param_limits = limits[:]  # must be list of tuples of (min, max) limits
        self._param_representations = None if repr_map is None else repr_map.copy()  # must be list of dictionaries mapping value to str, or None for direct integer display
        self.active_param = 0

    @property
    def len(self):
        return len(self._params)

    def increment(self, i=1):
        self._params[self.active_param] = min(self._param_limits[self.active_param][1], self._params[self.active_param] + 1)

    def decrement(self, i=1):
        self._params[self.active_param] = max(self._param_limits[self.active_param][0], self._params[self.active_param] - 1)

    def get_str(self, i):
        s = self._param_names[i] + ': '
        if self._param_representations is None or self._param_representations[i] is None:
            s += str(self._params[i])
        else:
            s += self._param_representations[i][self._params[i]]
        return s


class MenuPage:
    def __init__(self, title, params):
        self._title = title
        self._params = params
        self.reset()

    def reset(self):
        self.current_selection = 0
        self._params.active_param = 0

    def select(self):
        self.current_selection += 1
        if self.current_selection >= len(self._params._params) + 1:  # +1 for title line
            self.current_selection = 0
        if self.current_selection > 0:  # selected a value instead of title
            self._params.active_param = self.current_selection - 1

    def decrease_value(self):
        self._params.decrement()

    def increase_value(self):
        self._params.increment()

    def _sel(self, i):
        return ">" if self.current_selection == i else " "

    def _draw_param(self, i):
        return ' {}{}'.format(self._sel(i), self._params.get_str(i))

    def draw(self):
        s = '{sel}{title}'.format(sel=self._sel(0), title=self._title)
        for i in range(self._params.len):
            s += '\n {sel}{p}'.format(sel=self._sel(i + 1), p=self._params.get_str(i))

        staticwin.clear()
        staticwin.addstr(1, 1, s, curses.color_pair(1))
        staticwin.box()
        staticwin.refresh()

osc_list_param_names = ['Status', 'Waveform', 'Frequency', 'Sync', 'Ring']
osc_list_init_values = [0, 0, 440, 0, 0]
osc_list_limits = [(0, 1), (0, 3), (100, 500), (0, 1), (0, 1)]
osc_list_repr_map = [{0: 'dis', 1: 'en'}, {0: 'SIN', 1: 'TRI', 2: 'SQU', 3: 'NSE'}, None, {0: 'No sync', 1: 'sync'}, {0: 'no ringmod', 1: 'ringmod'}]

pages = [
    MenuPage('Osc 1', PageParams(osc_list_param_names, osc_list_init_values, osc_list_limits, osc_list_repr_map)),
    MenuPage('Osc 2', PageParams(osc_list_param_names, osc_list_init_values, osc_list_limits, osc_list_repr_map)),
    MenuPage('Osc 3', PageParams(osc_list_param_names, osc_list_init_values, osc_list_limits, osc_list_repr_map)),
    MenuPage('Filter', PageParams(['Type', 'Cutoff freq', 'Resonance'], [1, 1000, 1], [(0, 4), (100, 10000), (0, 10)], [{0: 'off', 1: 'LP', 2: 'BP', 3: 'HP', 4: '3off'}, None, None])),
    MenuPage('Arpegiator', PageParams(['Status', 'Mode'], [0, 0], [(0, 1), (0, 3)], [{0: 'dis', 1: 'en'}, {0: 'up', 1: 'down', 2: 'pingpong', 3: 'random'}])),
]
menu = Menu(pages)


def main_loop():
    while True:
        menu.draw()

        in_c = staticwin.getch()
        if in_c == ord('q') or in_c == ord('Q'):
            break
        elif in_c == ord('-'):
            menu.rotate_left()
        elif in_c == ord('+') or in_c == ord('='):
            menu.rotate_right()
        elif in_c == ord(' '):
            menu.select()


try:
    main_loop()
except Exception as e:
    # reset console now to get proper backtrace output
    curses.echo()
    curses.nocbreak()
    curses.curs_set(1)
    raise e
else:
    # reset console
    curses.echo()
    curses.nocbreak()
    curses.curs_set(1)
