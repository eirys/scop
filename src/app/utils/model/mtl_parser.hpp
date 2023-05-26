/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mtl_parser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/26 13:15:11 by etran             #+#    #+#             */
/*   Updated: 2023/05/26 15:06:36 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "material.hpp"

namespace scop {
namespace mtl {

/**
 * MtlParser for .mtl files.
*/
class MtlParser {
public:
	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	MtlParser() = default;
	MtlParser(MtlParser&& x) = default;
	~MtlParser() = default;

	MtlParser(const MtlParser& x) = delete;
	MtlParser&	operator=(const MtlParser& x) = delete;

	/* ========================================================================= */

	/**
	 * @brief Parse a .mtl file.
	 *
	 * @param path Path to the .mtl file.
	*/
	Material			parseFile(const std::string& path);

private:
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	Material			material_output;
	size_t				current_pos;
	std::string			line;
	std::string			token;

	/* ========================================================================= */
	/*                               CONST MEMBERS                               */
	/* ========================================================================= */

	const std::string	whitespaces = " \t\n \r";

	/* ========================================================================= */
	/*                                 EXCEPTIONS                                */
	/* ========================================================================= */

	class parse_error: public std::exception {
		public:
			parse_error() = delete;
			parse_error(const std::string& msg): _msg(msg) {}

			const char* what() const noexcept override {
				return _msg.c_str();
			}

		private:
			const std::string _msg;
	};

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	void				checkFile(const std::string& path) const;
	void				processLine();

	bool				getWord();
	void				skipComment() noexcept;

}; // class MtlParser

} // namespace mtl
} // namespace scop