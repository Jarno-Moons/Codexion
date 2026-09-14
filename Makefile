NAME        = codexion

CC          = cc
CFLAGS      = -Wall -Wextra -Werror -pthread -Iincludes

SRC_DIR     = src

SRCS        = \
$(SRC_DIR)/main.c \
$(SRC_DIR)/parser.c \
$(SRC_DIR)/simulation.c \
$(SRC_DIR)/monitor.c \
$(SRC_DIR)/coder.c \
$(SRC_DIR)/cleanup.c \
$(SRC_DIR)/sim_state.c \
$(SRC_DIR)/scheduler/heap.c \
$(SRC_DIR)/scheduler/priority.c \
$(SRC_DIR)/scheduler/sync.c \
$(SRC_DIR)/scheduler/request.c \
$(SRC_DIR)/utils/ft_strtoll.c \
$(SRC_DIR)/utils/error.c \
$(SRC_DIR)/utils/time.c \
$(SRC_DIR)/utils/init_resources.c

OBJS        = $(SRCS:.c=.o)

HEADER      = includes/codexion.h

RED         = \033[0;31m
GREEN       = \033[0;32m
YELLOW      = \033[0;33m
RESET       = \033[0m

all: $(NAME)

$(NAME): $(OBJS)
	@printf "\n$(YELLOW)Linking $(NAME)...$(RESET)\n"
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@printf "\n$(GREEN)$(NAME) successfully built!$(RESET)\n"

# compile rule (works with subfolders too)
%.o: %.c $(HEADER)
	@printf "$(YELLOW)Compiling $<...$(RESET)"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@printf "$(RED)Cleaning object files...$(RESET)\n"
	rm -f $(OBJS)

fclean: clean
	@printf "$(RED)Cleaning executable...$(RESET)\n"
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
